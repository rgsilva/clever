/**
 * Clever programming language
 * Copyright (c) 2011-2012 Clever Team
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
 */

#include "core/clever.h"
#include "core/opcode.h"

namespace clever {

#ifdef CLEVER_DEBUG
const char* get_opcode_name(Opcode opnum)
{
#define CASE(s) case s: return #s
	switch (opnum) {
		CASE(OP_RET);
		CASE(OP_ASSIGN);
		CASE(OP_ADD);
		CASE(OP_SUB);
		CASE(OP_MUL);
		CASE(OP_DIV);
		CASE(OP_MOD);
		CASE(OP_JMP);
		CASE(OP_FCALL);
		CASE(OP_TCALL);
		CASE(OP_END_THREAD);
		CASE(OP_LEAVE);
		CASE(OP_SEND_VAL);
		CASE(OP_JMPZ);
		CASE(OP_PRE_INC);
		CASE(OP_PRE_DEC);
		CASE(OP_POS_INC);
		CASE(OP_POS_DEC);
		EMPTY_SWITCH_DEFAULT_CASE();
	}
#undef CASE
	return "UNKNOWN";
}
#endif

} // clever
