/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_component_manager_agent_init.hpp"

#include <sc-config/sc_config.hpp>

#include "utils/sc_component_manager_command_init.hpp"

using namespace initModule;
using namespace keynodes;

ScResult ScComponentManagerInitAgent::DoProgram(ScAction & action)
{
  ScConfig config{ScMemory::ms_configPath, {PathKeysOfConfigPath::KB_PATH}};
  ScConfigGroup configManager = config["sc-component-manager"];

  ScComponentManagerCommandInit command = ScComponentManagerCommandInit(configManager[PathKeysOfConfigPath::KB_PATH]);

  ScAddrUnorderedSet const & specifications = command.Execute(&m_context, action);
  ScStructure result = m_context.GenerateStructure();
  for (auto const & specification : specifications)
    result << specification;
  action.SetResult(result);

  return action.FinishSuccessfully();
}

ScAddr ScComponentManagerInitAgent::GetActionClass() const
{
  return keynodes::ScComponentManagerKeynodes::action_components_init;
}
