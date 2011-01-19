/*
 * Clever programming language
 * Copyright (c) 2011 Clever Team
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * $Id: compiler.cc 315 2011-01-18 23:24:26Z felipensp $
 */

#include <iostream>
#include "pkgmanager.h"
#include "cstring.h"
#include "std/package.h"

namespace clever {

/**
 * Loads native packages
 */
void PackageManager::Init(FunctionTable* ftable) throw() {
	m_ftable = ftable;

	addPackage(CSTRING("std"), std_pkg::g_std_package);
}

/**
 * Load an entire package
 */
void PackageManager::loadPackage(const CString* package) throw() {
	PackageMap::const_iterator it = m_packages.find(package);

	if (it != m_packages.end()) {
		/**
		 * Initializes the package
		 */
		it->second->Init();

		{
			ModuleMap& modules = it->second->get_modules();
			ModuleMap::const_iterator it = modules.begin(), end = modules.end();

			while (it != end) {
				loadModule(it->second);
				++it;
			}
		}
	} else {
		std::cerr << "package '" << *package << "' not found" << std::endl;
	}
}

/**
 * Loads an specific module
 */
void PackageManager::loadModule(Module* module) throw() {
	/**
	 * Initializes the module
	 */
	module->Init();

	{
		FunctionMap& funcs = module->get_functions();
		FunctionMap::const_iterator it = funcs.begin(), end = funcs.end();


		/**
		 * Inserts the function into the global function table
		 */
		while (it != end) {
			m_ftable->insert(std::pair<const std::string, FunctionPtr>(it->first, it->second));
			++it;
		}
	}
}

/**
 * Loads an specific module package by supplying the package and module names
 */
void PackageManager::loadModule(const CString* package, const CString* module) throw() {
	PackageMap::const_iterator it = m_packages.find(package);

	if (it != m_packages.end()) {
		/**
		 * Initializes the package
		 */
		it->second->Init();

		{
			ModuleMap& modules = it->second->get_modules();
			ModuleMap::const_iterator it_mod = modules.find(module);

			if (it_mod != modules.end()) {
				loadModule(it_mod->second);
			}
		}
	}
}

/**
 * Removes the packages and its modules
 */
void PackageManager::shutdown() throw() {
	PackageMap::const_iterator it = m_packages.begin(), end = m_packages.end();

	while (it != end) {
		ModuleMap& modules = it->second->get_modules();
		ModuleMap::const_iterator it2 = modules.begin(), end2 = modules.end();

		while (it2 != end2) {
			delete it2->second;
			++it2;
		}
		delete it->second;
		++it;
	}
}

} // clever