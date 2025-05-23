# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.2.0] - 09.03.2025

### Changed

- Use generator Ninja in CMakePresets.json
- Rename Conan remote repository url to https://conan.ostis.net/artifactory/api/conan/ostis-ai-library

### Fixed

- Distribute tests by modules
- Make init-lib and console-interface as object libraries to avoid problems of dynamic loading

## [0.1.0] - 12.02.2025

### Added

- Intro for sc-component-manager in docs
- CMake flags section in docs
- Quick start section for users in docs
- Quick start section for Contributors in docs
- Describe myself decomposition
- Add components logging
- Update clang-format
- Agents for init, search, install commands
- Documentation of CLion cmake-configuration
- Download components from a separate directory of a repository, not to clone all repository 
- Add repository url parser to get information from GitHub repository link
- Add installation of kb/ps/interface components to the relevant directories
- Add quiet installation mode
- Add SCn documentation
- Add SCn documentation environment
- Add contributing document
- Add codestyle document
- Add git workflow
- Add changelog
- Add license
- Add utils
- Add loading for downloaded scs files
- Add logging
- Add components install command
- Add components search command
- Add components init command
- Add storage config and config parser
- Create base environment to run commands
- Add commands redaction 
- Add new search flags by note, purpose, main identifier, key sc-element

### Changed

- Build sc-component-manager with conan-provided dependencies
- Install sc-machine with conan
- Rename questions to actions
- Make sc-component-manager as shared library
- Separated envs from envs of sc-machine
- Replace ExecutionResult with bool and update logging.
- Decompose install command to functions
- Move storage configs to kb

### Fixed

- Check console pool for new strings; join thread with getline
- Processing commands with multiple string spaces
- Install a component while running `components install [name]` with no `--idtf` flag
- Exception while running `components search --explanation` with an empty substring
- Use nrel_authors instead of nrel_author

### Removed

- Remove codegen for agents
- Remove svn dependency
- Remove trunk folder when download git repository
- Remove storage parser
