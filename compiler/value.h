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

#ifndef CLEVER_VALUE_H
#define CLEVER_VALUE_H

#include <cstring>
#include "compiler/clever.h"
#include "compiler/refcounted.h"
#include "types/type.h"
#include "types/native_types.h"
#include "compiler/cstring.h"

namespace clever {

class Value : public RefCounted {
public:
	union DataValue {
		long lval;
		double dval;
		const CString* sval;
		void* obj;

		DataValue() : lval(0) {}
		DataValue(long value) : lval(value) {}
		DataValue(double value) : dval(value) {}
		DataValue(const CString* value) : sval(value) {}
		DataValue(void* value) : obj(value) {}
	};

	Value() : m_data(), m_type(NULL) {}
	Value(long n) : m_data(n), m_type(CLEVER_INT_TYPE) {}
	Value(double n) : m_data(n), m_type(CLEVER_DOUBLE_TYPE) {}
	Value(const CString* value) : m_data(value), m_type(CLEVER_STR_TYPE) {}

	~Value() {
		/* FIXME: Find a better way to check if m_data.obj is used */
		if (m_type && m_type != CLEVER_INT_TYPE && m_type != CLEVER_DOUBLE_TYPE) {
			const_cast<Type*>(m_type)->deallocData(m_data.obj);
		}
	}

	void setType(const Type* type) { m_type = type; }
	const Type* getType() const { return m_type; }

	void dump() const {
		if (m_type) {
			m_type->dump(&m_data);
		}
	}

	void setObj(void* ptr) { m_data.obj = ptr; }
	void* getObj() const { return m_data.obj; }

	void setInt(long n) { m_data.lval = n; m_type = CLEVER_INT_TYPE; }
	long getInt() const { return m_data.lval; }

	const DataValue* getData() const { return &m_data; }

	const CString* getStr() const { return m_data.sval; }

	void copy(Value* value) {
		m_type = value->getType();
		memcpy(&m_data, value->getData(), sizeof(DataValue));
	}
private:
	DataValue m_data;
	const Type* m_type;
};

} // clever

#endif // CLEVER_VALUE_H
