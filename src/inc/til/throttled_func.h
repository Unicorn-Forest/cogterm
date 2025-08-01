// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

namespace til
{
    namespace details
    {
        template<typename... Args>
        class throttled_func_storage
        {
        public:
            template<typename... MakeArgs>
            bool emplace(MakeArgs&&... args)
            {
                std::unique_lock guard{ _lock };
                const bool hadValue = _pendingRunArgs.has_value();
                _pendingRunArgs.emplace(std::forward<MakeArgs>(args)...);
                return hadValue;
            }

            template<typename F>
            void modify_pending(F f)
            {
                std::unique_lock guard{ _lock };
                if (_pendingRunArgs)
                {
                    std::apply(f, *_pendingRunArgs);
                }
            }

            void apply(const auto& func)
            {
                decltype(_pendingRunArgs) args;
                {
                    std::unique_lock guard{ _lock };
                    args = std::exchange(_pendingRunArgs, std::nullopt);
                }
                // Theoretically it should always have a value, because the throttled_func
                // should not call the callback without there being a reason.
                // But in practice a failure here was observed at least once.
                // It's unknown to me what caused it, so the best we can do is avoid a crash.
                assert(args.has_value());
                if (args)
                {
                    std::apply(func, *args);
                }
            }

            explicit operator bool() const
            {
                std::shared_lock guard{ _lock };
                return _pendingRunArgs.has_value();
            }

        private:
            // std::mutex uses imperfect Critical Sections on Windows.
            // --> std::shared_mutex uses SRW locks that are small and fast.
            mutable std::shared_mutex _lock;
            std::optional<std::tuple<Args...>> _pendingRunArgs;
        };

        template<>
        class throttled_func_storage<>
        {
        public:
            bool emplace()
            {
                return _isPending.exchange(true, std::memory_order_relaxed);
            }

            void apply(const auto& func)
            {
                if (_isPending.exchange(false, std::memory_order_relaxed))
                {
                    func();
                }
            }

            void reset()
            {
                _isPending.store(false, std::memory_order_relaxed);
            }

            explicit operator bool() const
            {
                return _isPending.load(std::memory_order_relaxed);
            }

        private:
            std::atomic<bool> _isPending;
        };
    } // namespace details

    template<typename... Args>
    struct throttle_options {
        using duration = std::chrono::duration<int64_t, std::ratio<1, 10000000>>;
        duration delay;
        bool leading = true;
        bool trailing = true;
        bool debounce = false;

        throttle_options(duration d) : delay(d) {}
    };

    template<typename... Args>
    class throttled_func
    {
    public:
        using filetime_duration = typename throttle_options<Args...>::duration;
        using function = std::function<void(Args...)>;

        // Throttles invocations to the given `func` to not occur more often than specified in options.
        //
        // Options:
        // * delay: The minimum time between invocations
        // * leading: If true, `func` will be invoked immediately on first call
        // * trailing: If true, `func` will be invoked after the delay
        // * debounce: If true, resets the timer on each call
        //
        // At least one of leading or trailing must be true.
        throttled_func(const throttle_options<Args...>& options, function func) :
            _func{ std::move(func) },
            _timer{ _createTimer() },
            _leading(options.leading),
            _trailing(options.trailing),
            _debounce(options.debounce)
        {
            if (!options.leading && !options.trailing) {
                throw std::invalid_argument("at least one of leading or trailing must be true");
            }

            const auto d = -options.delay.count();
            if (d >= 0)
            {
                throw std::invalid_argument("non-positive delay specified");
            }

            memcpy(&_delay, &d, sizeof(d));
        }

        // throttled_func uses its `this` pointer when creating _timer.
        // Since the timer cannot be recreated, instances cannot be moved either.
        throttled_func(const throttled_func&) = delete;
        throttled_func& operator=(const throttled_func&) = delete;
        throttled_func(throttled_func&&) = delete;
        throttled_func& operator=(throttled_func&&) = delete;

        // Throttles the invocation of the function passed to the constructor.
        //
        // If Debounce is true and you call this function again before the
        // underlying timer has expired, its timeout will be reset.
        //
        // If Leading is true and you call this function again before the
        // underlying timer has expired, the new arguments will be used.
        template<typename... MakeArgs>
        void operator()(MakeArgs&&... args)
        {
            const auto hadValue = _storage.emplace(std::forward<MakeArgs>(args)...);

            if (_debounce || !hadValue)
            {
                SetThreadpoolTimerEx(_timer.get(), &_delay, 0, 0);
            }

            if (_leading && !hadValue)
            {
                _func();
            }
        }

        // Modifies the pending arguments for the next function
        // invocation, if there is one pending currently.
        //
        // `func` will be invoked as func(Args...). Make sure to bind any
        // arguments in `func` by reference if you'd like to modify them.
        template<typename F>
        void modify_pending(F func)
        {
            _storage.modify_pending(func);
        }

        // Makes sure that the currently pending timer is executed
        // as soon as possible and in that case waits for its completion.
        // You can use this function in your destructor to ensure that any
        // pending callback invocation is completed as soon as possible.
        //
        // NOTE: Don't call this function if the operator()
        //       could still be called concurrently.
        void flush()
        {
            WaitForThreadpoolTimerCallbacks(_timer.get(), true);
            _timer_callback(nullptr, this, nullptr);
        }

    private:
        static void __stdcall _timer_callback(PTP_CALLBACK_INSTANCE /*instance*/, PVOID context, PTP_TIMER /*timer*/) noexcept
        try
        {
            const auto self = static_cast<throttled_func*>(context);
            if (self->_leading && !self->_trailing)
            {
                self->_storage.reset();
            }
            else if (self->_trailing)
            {
                self->_storage.apply(self->_func);
            }
        }
        CATCH_LOG()

        wil::unique_threadpool_timer _createTimer()
        {
            wil::unique_threadpool_timer timer{ CreateThreadpoolTimer(&_timer_callback, this, nullptr) };
            THROW_LAST_ERROR_IF(!timer);
            return timer;
        }

        FILETIME _delay;
        function _func;
        wil::unique_threadpool_timer _timer;
        details::throttled_func_storage<Args...> _storage;
        bool _leading;
        bool _trailing;
        bool _debounce;
    };
} // namespace til
