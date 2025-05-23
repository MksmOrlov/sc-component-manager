/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <gtest/gtest.h>

#include <memory>

#include <sc-memory/sc_memory.hpp>

#include <console-interface/sc_component_manager_command_parser.hpp>

class ScComponentManagerTest : public testing::Test
{
protected:
  void SetUp() override
  {
    m_commandParser = std::make_unique<ScComponentManagerParser>();
  }

  void TearDown() override {}

protected:
  std::unique_ptr<ScComponentManagerParser> m_commandParser;
};
