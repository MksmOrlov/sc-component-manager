/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>

#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"

#include "common-module/module/utils/common_utils.hpp"
#include "common-module/module/keynodes/ScComponentManagerKeynodes.hpp"

#include "init-module/utils/sc_component_manager_command_init.hpp"
#include "search-module/utils/sc_component_manager_command_search.hpp"
#include "install-module/utils/sc_component_manager_command_install.hpp"

class ScComponentManagerCommandHandler
{
public:
  explicit ScComponentManagerCommandHandler(std::map<ScAddr, std::string, ScAddrLessFunc> componentsPath)
    : m_componentsPath(std::move(componentsPath))
  {
    m_context = new ScMemoryContext();
    std::string const & specificationPath =
        m_componentsPath.at(keynodes::ScComponentManagerKeynodes::concept_reusable_kb_component);
    m_actions = {
        {"init", new ScComponentManagerCommandInit(specificationPath)},
        {"search", new ScComponentManagerCommandSearch()},
        {"install", new ScComponentManagerCommandInstall(m_componentsPath)}};
  }

  bool Handle(std::string const & commandType, CommandParameters const & commandParameters)
  {
    bool executionResult;
    m_commandParameters = commandParameters;
    auto const & it = m_actions.find(commandType);
    if (it != m_actions.end())
    {
      ScAddr actionAddrClass;
      try
      {
        actionAddrClass = common_utils::CommonUtils::managerParametersWithAgentRelations.at(it->first);
        SC_LOG_DEBUG("ScComponentManagerCommandHandler: execute " + it->first + " command");
      }
      catch (std::out_of_range const & ex)
      {
        SC_LOG_ERROR(ex.what());
      }
      ScAddr actionAddr = utils::AgentUtils::formActionNode(m_context, actionAddrClass, {});
      common_utils::CommonUtils::TranslateFromStringToScMemory(*m_context, actionAddr, commandParameters);

      utils::AgentUtils::applyAction(m_context, actionAddr, 30000);

      executionResult = m_context->HelperCheckEdge(
          scAgentsCommon::CoreKeynodes::action_finished_successfully, actionAddr, ScType::EdgeAccessConstPosPerm);
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionParseError, "Unsupported command type \"" + commandType + "\"");
    }

    return executionResult;
  }

  ~ScComponentManagerCommandHandler()
  {
    m_context->Destroy();
    delete m_context;

    for (auto const & it : m_actions)
      delete it.second;

    m_actions.clear();
  }

protected:
  ScMemoryContext * m_context{};
  CommandParameters m_commandParameters;
  std::map<ScAddr, std::string, ScAddrLessFunc> m_componentsPath;
  std::map<std::string, ScComponentManagerCommand *> m_actions;
};
