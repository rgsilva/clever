/**
 * Clever programming language
 * Copyright (c) Clever Team
 *
 * This file is distributed under the MIT license. See LICENSE for details.
 */

#include "core/cstring.h"
#include "core/compiler.h"
#include "core/scope.h"
#include "core/value.h"
#include "core/location.hh"
#include "core/astdump.h"
#include "core/codegen.h"
#include "core/evaluator.h"
#include "core/resolver.h"

namespace clever {

// Declaration of namespace global type ptrs for Clever native types
DECLARE_CLEVER_NATIVE_TYPES();

/// Compiler initialization phase
void Compiler::init()
{
	// Add 'null' to the constant pool
	addConstant(new Value());

	m_pkg.init();
}

/// Frees all resource used by the compiler
void Compiler::shutdown()
{
	m_pkg.shutdown();

	CLEVER_SAFE_DELETE(g_cstring_tbl);

	ValuePool::const_iterator it2 = m_const_pool.begin(),
		end2 = m_const_pool.end();

	while (it2 != end2) {
		CLEVER_SAFE_DELREF(*it2);
		++it2;
	}

	ValuePool::const_iterator it3 = m_tmp_pool.begin(),
		end3 = m_tmp_pool.end();

	while (it3 != end3) {
		CLEVER_SAFE_DELREF(*it3);
		++it3;
	}

	if (m_scope_pool.size()) {
		CLEVER_SAFE_DELETE(m_scope_pool[0]);
	}
}

/// Displays an error message and exits
void Compiler::error(const char* msg)
{
	std::cerr << "Compile error: " << msg << std::endl;
	CLEVER_EXIT_FATAL();
}

/// Displays an error message
void Compiler::error(const std::string& message, const location& loc)
{
	if (loc.begin.filename) {
		std::cerr << "Compile error: " << message << " on "
			<< *loc.begin.filename << " line " << loc.begin.line << "\n";
	} else {
		std::cerr << "Compile error: " << message << " on line "
			<< loc.begin.line << "\n";
	}
	CLEVER_EXIT_FATAL();
}

/// Displays a formatted error message and abort the compilation
void Compiler::errorf(const location& loc, const char* format, ...)
{
	std::ostringstream out;
	va_list args;

	va_start(args, format);

	vsprintf(out, format, args);

	va_end(args);

	error(out.str(), loc);
}

void Compiler::emitAST(ast::Node* tree)
{
	if (tree) {
		if (m_flags & USE_OPTIMIZER) {
			ast::Evaluator evaluator;
			tree = tree->accept(evaluator);
		}
		if (m_flags & DUMP_AST) {
			ast::Dumper astdump;
			tree->accept(astdump);
		}

		ast::Resolver resolver(this);
		tree->accept(resolver);

		m_scope_pool = resolver.getSymTable()->flatten();

		for (size_t i = 0; i < m_scope_pool.size(); i++) {
			m_scope_pool[i]->setId(i);
		}

		ast::Codegen codegen(m_ir, this);
		tree->accept(codegen);

		m_ir.push_back(IR(OP_HALT));

		delete tree;
	}
}

/// Adds a new constant value to the constant pool
size_t Compiler::addConstant(Value* value)
{
	m_const_pool.push_back(value);

	return m_const_id++;
}

/// Adds a new temporary value to the temporary pool
size_t Compiler::getTempValue() {
	m_tmp_pool.push_back(new Value());

	return m_tmp_id++;
}

} // clever
