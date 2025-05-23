/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <vector>
#include <map>

class SpecificationConstants
{
public:
  static std::string const SPECIFICATION_FILENAME;
  static std::string const SCS_EXTENSION;
  static char const DIRECTORY_DELIMITER;
};

class GoogleDriveConstants
{
public:
  static std::string const GOOGLE_DRIVE_PREFIX;
  static std::string const GOOGLE_DRIVE_FILE_PREFIX;
  static std::string const GOOGLE_DRIVE_DOWNLOAD_PREFIX;
  static std::string const GOOGLE_DRIVE_POSTFIX;
};

class GitHubConstants
{
public:
  static std::string const GIT_CLONE;
  static std::string const FLAG_NO_CHECKOUT;
  static std::string const FLAG_DEPTH;
  static std::string const FLAG_FILTER_TREE;
  static std::string const GIT_SPARSE_CHECKOUT;
  static std::string const GIT_CHECKOUT;
  static std::string const RAW_GITHUB_PREFIX;
  static std::string const GITHUB_PREFIX;
  static std::string const TREE;
  static std::string const GREP_DEFAULT_BRANCH_COMMAND;
  static std::string const GITHUB_DOWNLOAD_FILE_COMMAND_PREFIX;
  static std::string const GITHUB_GET_DEFAULT_BRANCH_COMMAND_PREFIX;
};

class CommandConstants
{
public:
  static std::string const COMPONENTS_COMMAND_PREFIX;
  static std::string const COMPONENTS_COMMAND_INIT;
  static std::string const COMPONENTS_COMMAND_INSTALL;
  static std::string const COMPONENTS_COMMAND_SEARCH;
  static std::map<std::string, std::string> COMMAND_MAP;
};

class CommandsConstantsFlags
{
public:
  static std::string const IDTF;
  static std::string const SET;
  static std::string const EXPLANATION;
  static std::string const AUTHOR;
  static std::string const CLASS;
  static std::string const NOTE;
  static std::string const PURPOSE;
  static std::string const KEY;
  static std::string const MAIN_ID;
};

class PathKeysOfConfigPath
{
public:
  static std::string const KB_PATH;
  static std::string const PS_PATH;
  static std::string const UI_PATH;
};
