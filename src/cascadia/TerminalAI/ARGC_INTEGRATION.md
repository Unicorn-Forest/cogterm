# Argc Integration in Windows Terminal

This document describes the Phase 3 Argc integration implemented in Windows Terminal as part of the AI Integration Roadmap.

## Overview

The Argc integration adds intelligent command parsing and autocompletion capabilities to Windows Terminal by leveraging the Argc Bash framework for CLI development.

## Features

### 1. ArgcParser Module (`src/cascadia/TerminalAI/ArgcParser.h/cpp`)

The core component that provides:
- **Command Parsing**: Intelligent parsing of command lines using Argc comment-based rules
- **Completion Suggestions**: Context-aware autocompletion for commands, flags, and options
- **Help Generation**: Automatic help text generation for recognized commands
- **Built-in Command Support**: Pre-configured support for common commands (ls, grep, git)

#### Key Methods:
- `ParseCommand()`: Parse command line into structured components
- `GetCompletions()`: Get intelligent completion suggestions
- `GenerateHelp()`: Generate help text for commands
- `IsValidCommand()`: Check if a command is recognized

### 2. Command Palette Integration (`src/cascadia/TerminalApp/CommandPalette.h/cpp`)

Enhanced the Command Palette with Argc-powered suggestions:
- **AI-Powered Filtering**: Adds Argc completions to command palette suggestions
- **Intelligent Search**: Context-aware command suggestions based on partial input
- **Priority-Based Ranking**: Completions are ranked by relevance and type

#### Integration Points:
- `_collectFilteredActions()`: Enhanced to include Argc suggestions
- `_getArgcSuggestions()`: Generates command suggestions for the palette
- `_initializeArgcParser()`: Initializes the Argc parser on startup

### 3. Terminal Control Autocompletion (`src/cascadia/TerminalControl/TermControl.h/cpp`)

Added Tab-based autocompletion to the terminal control:
- **Tab Completion**: Press Tab to trigger Argc autocompletion
- **Current Line Analysis**: Extracts current command line for completion
- **Intelligent Insertion**: Inserts the most relevant completion

#### Key Features:
- Tab key handling in `_KeyHandler()`
- Current line extraction with `_GetCurrentCommandLine()`
- Completion display via `_ShowArgcCompletions()`

## Architecture

```
TerminalApp (Command Palette)
    ↓ uses
TerminalAI (ArgcParser)
    ↑ used by
TerminalControl (Tab Completion)
```

## Usage

### Command Palette
1. Open Command Palette (Ctrl+Shift+P)
2. Start typing a command (e.g., "ls", "git")
3. See Argc-powered suggestions with descriptions

### Terminal Autocompletion
1. Type a partial command in the terminal
2. Press Tab to trigger autocompletion
3. The most relevant completion will be inserted

## Supported Commands

Currently supports built-in definitions for:
- **ls**: List directory contents with flags (-l, -a, -h) and options (--color)
- **grep**: Text pattern searching with flags (-i, -v, -n) and options (--include, --exclude)
- **git**: Version control with subcommands (add, commit, push, pull, status)

## Extension Points

The system is designed to be extensible:
- Add new command definitions via `AddCommandDefinition()`
- Load command definitions from external files via `LoadCommandDefinitions()`
- Support for custom Argc scripts with comment-based configuration

## Implementation Notes

### Design Decisions
1. **Minimal Integration**: The implementation focuses on core functionality without complex dependencies
2. **Graceful Degradation**: Features work even if Argc parser initialization fails
3. **Performance Conscious**: Autocompletion is triggered only on explicit user action (Tab)
4. **Extensible Architecture**: Easy to add new commands and completion sources

### Future Enhancements
1. **Full Argc Script Support**: Parse actual Argc scripts with comment tags
2. **Advanced Completion UI**: Show completion popup instead of direct insertion
3. **Command History Integration**: Learn from user command patterns
4. **Integration with AI Engine**: Connect with other AI features for smarter suggestions

## Testing

Unit tests are provided in `ArgcParserTests.cpp` covering:
- Basic initialization and command parsing
- Completion generation for commands and arguments
- Help text generation
- Command validation

## Error Handling

The implementation includes robust error handling:
- Graceful parser initialization failure
- Safe command line extraction
- Protected completion generation
- No crashes on invalid input

## Dependencies

- Windows Terminal core infrastructure
- TerminalAI module for AI integration
- Standard C++ libraries for string processing and containers

This Argc integration provides the foundation for intelligent command-line assistance while maintaining the performance and reliability expected from Windows Terminal.