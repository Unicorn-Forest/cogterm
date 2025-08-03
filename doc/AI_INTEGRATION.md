# AI Integration for Windows Terminal

This document describes the minimal AI integration implementation that provides the foundation for the comprehensive roadmap outlined in `AI_INTEGRATION_ROADMAP.md`.

## Overview

The minimal AI integration adds basic infrastructure to Windows Terminal that enables:

1. **AI Engine Framework** - Core interface and basic implementation for AI operations
2. **AI Connection Type** - New terminal connection that provides AI-powered interactions
3. **AI Settings Model** - Configuration system for AI features
4. **Basic AI Chat** - Simple conversational interface for terminal assistance

## Architecture

### Core Components

#### 1. TerminalAI Library (`src/cascadia/TerminalAI/`)

**AIEngine.h/cpp** - Core AI engine interface and basic implementation:
- `AIEngine` - Abstract base class for AI engines
- `BasicAIEngine` - Simple implementation for demonstration and testing
- Provides methods for command processing, chat, and function execution
- Event-driven architecture with response and error events

#### 2. AI Connection (`src/cascadia/TerminalConnection/AIConnection.*`)

**AIConnection.h/cpp** - Terminal connection for AI interactions:
- Implements `ITerminalConnection` interface
- Integrates with `BasicAIEngine` for AI processing
- Supports AI-specific commands (`ai help`, `ai chat <message>`, etc.)
- Processes regular commands with AI assistance

#### 3. AI Settings (`src/cascadia/TerminalSettingsModel/AISettings.*`)

**AISettings.h/cpp** - Configuration model for AI features:
- Provider settings (AI provider, API key, model)
- Feature toggles (function calling, chat, autocompletion)
- Directory settings for functions and agents
- JSON serialization support

## Usage

### Creating an AI Terminal

To create a new AI-powered terminal tab, you would configure a profile with the AI connection type:

```json
{
  "name": "AI Assistant",
  "connectionType": "AIConnection", 
  "aiProvider": "basic",
  "aiModel": "demo"
}
```

### AI Commands

Once connected to an AI terminal, you can use:

- `ai help` - Show available AI commands
- `ai chat <message>` - Start a chat with the AI
- `ai function <name> <args>` - Execute an AI function
- Any regular command - Gets processed with AI assistance

### Example Interaction

```
AI Terminal Connection Started
Provider: basic
Model: demo
Type 'ai help' for assistance or just enter commands normally.
AI> ai chat hello
AI: Hello! How can I help you with your terminal tasks?
AI> ls
This command lists directory contents. Use 'ls -la' for detailed listing.
AI> git status
Git command detected. Common operations: git status, git add, git commit, git push
AI>
```

## Configuration

AI settings can be configured in the terminal settings JSON:

```json
{
  "ai": {
    "aiProvider": "basic",
    "defaultModel": "demo",
    "enableFunctionCalling": true,
    "enableAIChat": true,
    "enableAutoCompletion": false,
    "functionsDirectory": "%USERPROFILE%\\.terminal\\functions",
    "agentsDirectory": "%USERPROFILE%\\.terminal\\agents"
  }
}
```

## Extension Points

This minimal implementation provides the foundation for the comprehensive features outlined in the roadmap:

1. **Additional AI Engines** - Can implement the `AIEngine` interface for different providers (OpenAI, Azure, local models)
2. **Advanced Connections** - Specialized connection types for different AI tools (aichat, argc, etc.)
3. **Function Calling** - Framework exists for implementing LLM function calling
4. **Agent System** - Settings model supports agent configuration
5. **UI Integration** - Connection framework supports custom UI elements

## Future Development

This implementation serves as Phase 1 of the roadmap, enabling incremental development of:

- **Phase 2**: Full aichat integration with real AI providers
- **Phase 3**: argc command parsing and intelligent autocompletion  
- **Phase 4**: llm-functions integration for tool calling
- **Phase 5**: clii JavaScript runtime integration
- **Phase 6**: Advanced UI features and settings pages
- **Phase 7**: Comprehensive testing and documentation

## Build Integration

The AI components are integrated into the existing build system:

- `TerminalAI.vcxproj` - Static library for AI engine components
- AI connection added to `TerminalConnection.vcxproj`
- AI settings added to `TerminalSettingsModel.vcxproj`
- Projects added to `OpenConsole.sln`

This provides a solid foundation for the ambitious AI integration roadmap while maintaining compatibility with existing terminal functionality.