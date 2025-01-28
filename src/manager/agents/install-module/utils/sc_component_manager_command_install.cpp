/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_component_manager_command_install.hpp"

#include <sc-memory/utils/sc_exec.hpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/GenerationUtils.hpp>

#include <common/sc_component_utils.hpp>
#include <common/common_utils.hpp>

using namespace common_utils;

ScComponentManagerCommandInstall::ScComponentManagerCommandInstall(
    std::map<ScAddr, std::string, ScAddrLessFunc> componentsPath)
  : m_componentsPath(std::move(componentsPath))
{
}

/**
 * @brief Check if components from specification is available
 * @param context current sc-memory context
 * @param componentsToInstall vector of components identifiers
 * @return vector of available components
 */
ScAddrUnorderedSet ScComponentManagerCommandInstall::GetAvailableComponents(
    ScMemoryContext * context,
    ScAddrUnorderedSet const & componentsToInstall)
{
  ScAddrUnorderedSet availableComponents;
  for (ScAddr const & componentToInstall : componentsToInstall)
  {
    try
    {
      SC_LOG_DEBUG(
          "ScComponentManagerCommandInstall: Validating component \""
          << context->GetElementSystemIdentifier(componentToInstall) << "\"");
      ValidateComponent(context, componentToInstall);
    }
    catch (utils::ScException const & exception)
    {
      SC_LOG_ERROR("ScComponentManagerCommandInstall: Unable to install component");
      SC_LOG_DEBUG(exception.Message());
      continue;
    }
    SC_LOG_DEBUG(
        "ScComponentManagerCommandInstall: Component \"" << context->GetElementSystemIdentifier(componentToInstall)
                                                         << "\" is specified correctly");
    availableComponents.insert(componentToInstall);
  }
  return availableComponents;
}

/**
 * @brief Installation of component
 * @param context current sc-memory context
 * @param componentAddr component sc-addr
 */
bool ScComponentManagerCommandInstall::InstallComponent(ScMemoryContext * context, ScAddr const & componentAddr)
{
  std::vector<std::string> scripts = componentUtils::InstallUtils::GetInstallScripts(context, componentAddr);

  // TODO: remove cycle after removing the init-command
  std::set<std::string> setOfScripts;
  for (std::string const & script : scripts)
  {
    setOfScripts.insert(script);
  }

  for (std::string const & script : setOfScripts)
  {
    std::string const & componentDirName =
        componentUtils::InstallUtils::GetComponentDirName(context, componentAddr, m_downloadDir);

    std::stringstream command;
    command << "cd " << componentDirName << " && " << "." << script;

    ScExec Exec({command.str()});
  }

  return true;
}

bool ScComponentManagerCommandInstall::EraseTempOutputEdges(ScMemoryContext * context, ScAddr const & node)
{
  bool result = context->IsElement(node);
  if (result)
  {
    ScIterator3Ptr const & edgesIterator = context->CreateIterator3(
        node,  // set of components
        ScType::ConstTempPosArc,
        ScType::ConstNode);  // component
    while (edgesIterator->Next())
    {
      context->EraseElement(edgesIterator->Get(1));
    }
  }
  return result;
}

ScAddrUnorderedSet ScComponentManagerCommandInstall::Execute(ScAgentContext * context, ScAddr const & actionAddr)
{
  ScAddr const & parameterNode = utils::IteratorUtils::getAnyByOutRelation(context, actionAddr, ScKeynodes::rrel_1);
  ScAddrUnorderedSet componentsToInstall = CommonUtils::GetComponentsToInstall(*context, parameterNode);
  if (componentsToInstall.empty())
  {
    SC_LOG_INFO("ScComponentManagerCommandInstall: No identifier provided, can't install");
    return componentsToInstall;
  }

  componentsToInstall = GetAvailableComponents(context, componentsToInstall);

  ScAddr decompositionAddr;
  bool executionResult = true;
  for (ScAddr const & componentAddr : componentsToInstall)
  {
    if (CommonUtils::CheckIfInstalled(*context, componentAddr))
    {
      SC_LOG_DEBUG("Component \"" << context->GetElementSystemIdentifier(componentAddr) << "\" is already installed");
      continue;
    }
    context->GenerateConnector(
        ScType::ConstTempPosArc, keynodes::ScComponentManagerKeynodes::current_components_to_install, componentAddr);
    executionResult &= InstallDependencies(context, componentAddr);
    executionResult &=
        EraseTempOutputEdges(context, keynodes::ScComponentManagerKeynodes::current_components_to_install);
    if (!executionResult)
    {
      return {};
    }
    executionResult &= DownloadComponent(context, componentAddr);
    executionResult &= InstallComponent(context, componentAddr);
    // TODO: need to process installation method from component specification in kb

    decompositionAddr = CommonUtils::GetSubsystemDecompositionAddr(*context, componentAddr);
    if (context->IsElement(decompositionAddr))
      context->GenerateConnector(ScType::ConstPermPosArc, decompositionAddr, componentAddr);
    else
      SC_LOG_WARNING(
          "Component \"" << context->GetElementSystemIdentifier(componentAddr)
                         << "\" can't be added in myself decomposition");
  }

  return componentsToInstall;
}

/**
 * @brief Checks if component is valid.
 * Checks if:
 * - component exist;
 * - component is reusable;
 * - component's address link is valid;
 * - component's installation method is valid;
 * Throw exception if failed
 */
void ScComponentManagerCommandInstall::ValidateComponent(ScMemoryContext * context, ScAddr const & componentAddr)
{
  // Check if component exist
  if (!componentAddr.IsValid())
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionAssert, "ScComponentManagerCommandInstall: Component not found. Unable to install");
  }

  // Check if component is a reusable component
  if (!componentUtils::InstallUtils::IsReusable(context, componentAddr))
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionAssert, "ScComponentManagerCommandInstall: Component is not a reusable component.");
  }

  // Find and check component address
  if (componentUtils::InstallUtils::GetComponentAddressStr(context, componentAddr).empty())
  {
    SC_THROW_EXCEPTION(utils::ExceptionAssert, "ScComponentManagerCommandInstall: Component address not found.");
  }

  // Find and check component installation method
  if (!componentUtils::InstallUtils::IsComponentInstallationMethodValid(context, componentAddr))
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionAssert, "ScComponentManagerCommandInstall: Component installation method not found.");
  }
}

ScAddr ScComponentManagerCommandInstall::CreateSetToInstallStructure(
    ScMemoryContext * context,
    ScAddr const & dependenciesSet)
{
  if (!context->IsElement(dependenciesSet))
  {
    return dependenciesSet;
  }
  ScAddr const & setsParameter = context->GenerateNode(ScType::ConstNode);
  ScAddr const & mainParameter = context->GenerateNode(ScType::ConstNode);
  utils::GenerationUtils::generateRelationBetween(
      context, mainParameter, setsParameter, keynodes::ScComponentManagerKeynodes::rrel_sets);
  context->GenerateConnector(ScType::ConstPermPosArc, setsParameter, dependenciesSet);

  return mainParameter;
}

ScAddr ScComponentManagerCommandInstall::CheckDependencyDuplication(
    ScMemoryContext * context,
    ScAddr const & currentInstallationComponentsAddr,
    ScAddr const & dependenciesSet)
{
  ScAddr recursiveDependency;
  if (!context->IsElement(currentInstallationComponentsAddr) && !context->IsElement(dependenciesSet))
  {
    return recursiveDependency;
  }

  ScIterator3Ptr const & componentsIterator =
      context->CreateIterator3(dependenciesSet, ScType::ConstPermPosArc, ScType::ConstNode);

  while (componentsIterator->Next())
  {
    if (context->CheckConnector(currentInstallationComponentsAddr, componentsIterator->Get(2), ScType::ConstTempPosArc))
    {
      return componentsIterator->Get(2);
    }
  }
  return recursiveDependency;
}

/**
 * Tries to install component dependencies.
 * @return Returns {DependencyIdtf1, DependencyIdtf2, ...}
 * if installation successfull, otherwise
 * returns empty vector.
 */
bool ScComponentManagerCommandInstall::InstallDependencies(ScAgentContext * context, ScAddr const & componentAddr)
{
  bool result = true;
  ScAddr const & dependenciesSet = utils::IteratorUtils::getAnyByOutRelation(
      context, componentAddr, keynodes::ScComponentManagerKeynodes::nrel_component_dependencies);
  ScAddrUnorderedSet const & componentDependencies =
      componentUtils::SearchUtils::GetComponentDependencies(context, componentAddr);

  if (componentDependencies.empty())
  {
    return result;
  }

  if (componentDependencies != GetAvailableComponents(context, componentDependencies))
  {
    SC_LOG_ERROR(
        "ScComponentManagerCommandInstall: Can't install dependencies of the component \""
        << context->GetElementSystemIdentifier(componentAddr) << "\"");
    return false;
  }

  ScAddr const & recursiveDependency = CheckDependencyDuplication(
      context, keynodes::ScComponentManagerKeynodes::current_components_to_install, dependenciesSet);
  if (context->IsElement(recursiveDependency))
  {
    SC_LOG_ERROR(
        "ScComponentManagerCommandInstall: Found recursive dependency with component "
        << context->GetElementSystemIdentifier(recursiveDependency) << ", can't install component "
        << context->GetElementSystemIdentifier(componentAddr));
    return false;
  }

  // Get component dependencies and install them recursively
  std::string dependencyIdtf;
  ScAddr const & mainParameter = CreateSetToInstallStructure(context, dependenciesSet);

  ScAction actionAddr = context->GenerateAction(keynodes::ScComponentManagerKeynodes::action_components_install);
  actionAddr.SetArguments(mainParameter);

  // Call install components agent
  actionAddr.InitiateAndWait(30000);

  result = actionAddr.IsFinishedSuccessfully();
  if (!result)
  {
    SC_LOG_ERROR("ScComponentManagerCommandInstall: Dependencies are not successfully installed");
  }

  return result;
}

/**
 * Tries to download component from Github
 */
bool ScComponentManagerCommandInstall::DownloadComponent(ScMemoryContext * context, ScAddr const & componentAddr)
{
  ScAddr componentClass;
  ScIterator3Ptr const & componentClassIterator =
      context->CreateIterator3(ScType::ConstNodeClass, ScType::ConstPermPosArc, componentAddr);
  while (componentClassIterator->Next())
  {
    componentClass = componentClassIterator->Get(0);
    if (m_componentsPath.count(componentClass) != 0)
    {
      m_downloadDir = m_componentsPath.at(componentClass);
      break;
    }
  }

  // Get url address from where to download component
  std::string urlAddress;
  ScAddr const & componentAddress = componentUtils::SearchUtils::GetComponentAddress(context, componentAddr);
  context->GetLinkContent(componentAddress, urlAddress);

  // Download component source code from GitHub repository
  std::shared_ptr<Downloader> m_downloader = std::make_shared<DownloaderGit>();
  bool result = m_downloader->DownloadRepository(m_downloadDir, urlAddress);

  // Load scs files from the new component directory
  std::stringstream downloadPath;
  RepositoryUrlParser parser;
  parser.Parse(urlAddress);
  std::string const repositoryName = parser.GetRepositoryName();
  std::string const directoryName = parser.GetDirectoryName();
  downloadPath << m_downloadDir << SpecificationConstants::DIRECTORY_DELIMITER << repositoryName
               << SpecificationConstants::DIRECTORY_DELIMITER << directoryName;
  if (!componentUtils::LoadUtils::LoadScsFilesInDir(
          context, downloadPath.str(), SpecificationConstants::SPECIFICATION_FILENAME))
  {
    SC_LOG_WARNING("ScComponentManagerCommandInstall: there is no *.scs files to load from " << downloadPath.str());
  }

  return result;
}
