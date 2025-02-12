/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_module.hpp>

class CommonModule : public ScModule
{
public:
  void Initialize(ScMemoryContext * context) override;

  void Shutdown(ScMemoryContext * context) override;
};
