/**
 * Clever programming language
 * Copyright (c) Clever Team
 *
 * This file is distributed under the MIT license. See LICENSE for details.
 */

#include "core/value.h"
#include "core/vm.h"
#include "modules/std/core/array.h"
#include "modules/std/core/function.h"

namespace clever {

::std::string ArrayType::toString(TypeObject* value) const
{
	ArrayObject* arr = static_cast<ArrayObject*>(value);
	ValueVector& vec = arr->getData();
	::std::ostringstream out;

	out << "[";

	for (size_t i = 0, j = vec.size(); i < j; ++i) {
		Value* val = vec.at(i);

		if (val) {
			val->dump(out);
		} else {
			out << "null";
		}
		if (i < j-1) {
			out << ", ";
		}
	}

	out << "]";

	return out.str();
}

// Subscript operator
CLEVER_TYPE_AT_OPERATOR(ArrayType::at_op)
{
	ValueVector& arr = clever_get_this(ArrayObject*)->getData();
	long size = arr.size();

	if (!index->isInt()) {
		clever_throw("Invalid array index type");
		return NULL;
	}

	if (index->getInt() < 0 || index->getInt() >= size) {
		clever_throw("Array index out of bound!");
		return NULL;
	}

	if (is_write) {
		if (!arr[index->getInt()]) {
			arr[index->getInt()] = new Value;
		}
	}

	Value* result = arr.at(index->getInt());

	clever_addref(result);

	return result;
}

// + operator
CLEVER_TYPE_OPERATOR(ArrayType::add)
{
	if (rhs->isArray()) {
		ArrayObject* arr  = clever_get_object(ArrayObject*, lhs);
		ArrayObject* arr2 = clever_get_object(ArrayObject*, rhs);

		if (result == lhs) {
			static_cast<ArrayObject*>(result->getObj())->append(arr2->getData());
		} else {
			ArrayObject* new_obj = new ArrayObject(arr->getData());

			new_obj->append(arr2->getData());

			result->setObj(this, new_obj);
		}
	}
}

// Array::Array([arg, ...])
CLEVER_METHOD(ArrayType::ctor)
{
	result->setObj(this, new ArrayObject(args));
}

// void Array::append([arg, ... ])
CLEVER_METHOD(ArrayType::append)
{
	if (!clever_check_args("*")) {
		return;
	}

	if (!args.empty()) {
		ArrayObject* arrobj = clever_get_this(ArrayObject*);

		for (size_t i = 0, j = args.size(); i < j; ++i) {
			arrobj->pushValue(args[i]);
		}
	}
	result->setNull();
}

// int Array::size()
CLEVER_METHOD(ArrayType::size)
{
	if (!clever_check_no_args()) {
		return;
	}

	result->setInt(clever_get_this(ArrayObject*)->getData().size());
}

// mixed Array::at(int position)
CLEVER_METHOD(ArrayType::at)
{
	if (!clever_check_args("i")) {
		return;
	}

	ValueVector& arr = clever_get_this(ArrayObject*)->getData();

	long num = args[0]->getInt();

	if (num < 0 || arr.size() <= size_t(num)) {
		result->setNull();
		return;
	}

	result->copy(arr.at(num));
}

// void Array::resize(Int size)
CLEVER_METHOD(ArrayType::resize)
{
	if (!clever_check_args("i")) {
		return;
	}

	ArrayObject* arr = clever_get_this(ArrayObject*);

	result->setNull();

	if (args[0]->getInt() < 0
		|| arr->getData().size() > size_t(args[0]->getInt())) {
		return;
	}

	arr->getData().resize(args[0]->getInt());
}

// void Array::reserve(Int size)
CLEVER_METHOD(ArrayType::reserve)
{
	if (!clever_check_args("i")) {
		return;
	}

	ArrayObject* arr = clever_get_this(ArrayObject*);

	result->setNull();

	if (args[0]->getInt() < 0) {
		return;
	}

	arr->getData().reserve(args[0]->getInt());
}

// Array Array::reverse()
// Returns the reverse of this array
CLEVER_METHOD(ArrayType::reverse)
{
	if (!clever_check_no_args()) {
		return;
	}

	ValueVector& vec = clever_get_this(ArrayObject*)->getData();
	ValueVector::reverse_iterator it(vec.rbegin()), end(vec.rend());
	ValueVector rev;

	while (it != end){
		rev.push_back((*it));
		++it;
	}

	result->setObj(this, new ArrayObject(rev));
}

// mixed Array.shift()
// Removes and returns the first element of the array
CLEVER_METHOD(ArrayType::shift)
{
	if (!clever_check_no_args()) {
		return;
	}

	ValueVector& vec = clever_get_this(ArrayObject*)->getData();

	if (vec.empty()) {
		result->setNull();
		return;
	}

	result->copy(vec[0]);
	vec[0]->delRef();

	vec.erase(vec.begin());
}

// mixed Array.pop()
// Removes and returns the last element of the array
CLEVER_METHOD(ArrayType::pop)
{
	if (!clever_check_no_args()) {
		return;
	}

	ValueVector& vec = clever_get_this(ArrayObject*)->getData();

	if (vec.empty()) {
		result->setNull();
		return;
	}

	result->copy(vec[vec.size()-1]);
	vec[vec.size()-1]->delRef();

	vec.erase(vec.end()-1);
}

// Array Array.range(Int start, Int end)
// Returns a range as a new array
CLEVER_METHOD(ArrayType::range)
{
	if (!clever_check_args("ii")) {
		return;
	}

	ValueVector& vec = clever_get_this(ArrayObject*)->getData();

	if (vec.empty()){
		result->setNull();
		return;
	}

	ValueVector ran;
	ValueVector::size_type start = args[0]->getInt(),
		end = args[1]->getInt(),
		size = vec.size();
	bool reverse = (start > end);

	while ((reverse ? (end <= start) : (start <= end))) {
		if (start > size || end > size) {
			break;
		}

		ran.push_back(vec[start]);

		if (reverse) {
			--start;
		} else {
			++start;
		}
	}

	result->setObj(this, new ArrayObject(ran));
}

// Array Array::each(Function callback)
// Calls function once for each element in the array, passing the element as the only parameter to function
// The return values from function are returned in the same order as the array
CLEVER_METHOD(ArrayType::each)
{
	if (!clever_check_args("f")) {
		return;
	}

	Function* func = static_cast<Function*>(args[0]->getObj());
	ValueVector& vec = clever_get_this(ArrayObject*)->getData();
	ValueVector results;

	for (size_t i = 0, j = vec.size(); i < j; ++i) {
		ValueVector tmp_args;

		tmp_args.push_back(vec[i]);

		results.push_back(const_cast<VM*>(clever->vm)->runFunction(func, tmp_args));
	}

	result->setObj(this, new ArrayObject(results));

	std::for_each(results.begin(), results.end(), clever_delref);
}

// mixed Array.erase(Int position)
// Removes from this array the element at position, returning the value
CLEVER_METHOD(ArrayType::erase)
{
	if (!clever_check_args("i")) {
		return;
	}

	ValueVector& vec = clever_get_this(ArrayObject*)->getData();

	if (vec.empty()) {
		return;
	}

	long num = args[0]->getInt();
	size_t length = vec.size();

	if (num >= 0 && size_t(num) < length) {
		result->copy(vec[num]);

		vec[num]->delRef();

		vec.erase(vec.begin() + num);
	}
}

CLEVER_METHOD(ArrayType::begin)
{
	if (!clever_check_no_args()) {
		return;
	}

	result->setObj(CLEVER_ARRAYITER_TYPE,
		new ArrayIteratorObject(clever_get_this(ArrayObject*)));
}

CLEVER_METHOD(ArrayType::end)
{
	if (!clever_check_no_args()) {
		return;
	}

	ArrayObject* arr = clever_get_this(ArrayObject*);

	result->setObj(CLEVER_ARRAYITER_TYPE,
		new ArrayIteratorObject(arr, arr->getData().end()));
}

// Type initialization
CLEVER_TYPE_INIT(ArrayType::init)
{
	setConstructor((MethodPtr)&ArrayType::ctor);

	addMethod(new Function("append",  (MethodPtr)&ArrayType::append));
	addMethod(new Function("size",    (MethodPtr)&ArrayType::size));
	addMethod(new Function("at",      (MethodPtr)&ArrayType::at));
	addMethod(new Function("resize",  (MethodPtr)&ArrayType::resize));
	addMethod(new Function("reserve", (MethodPtr)&ArrayType::reserve));
	addMethod(new Function("reverse", (MethodPtr)&ArrayType::reverse));
	addMethod(new Function("each",    (MethodPtr)&ArrayType::each));
	addMethod(new Function("shift",   (MethodPtr)&ArrayType::shift));
	addMethod(new Function("pop",     (MethodPtr)&ArrayType::pop));
	addMethod(new Function("range",   (MethodPtr)&ArrayType::range));
	addMethod(new Function("erase",	  (MethodPtr)&ArrayType::erase));

	addMethod(new Function("begin",	  (MethodPtr)&ArrayType::begin));
	addMethod(new Function("end",	  (MethodPtr)&ArrayType::end));
}

// ArrayIterator methods ///////////////////////////////////////////////////////

// ArrayIterator::ArrayIterator(Array arg)
// Initializes an iterator to arg.begin()
CLEVER_METHOD(ArrayIterator::ctor)
{
	if (!clever_check_args("a")) {
		return;
	}

	ArrayObject* arr_obj = static_cast<ArrayObject*>(args[0]->getObj());
	result->setObj(this, new ArrayIteratorObject(arr_obj));
}

// Gets the object associated with the current iterator
CLEVER_METHOD(ArrayIterator::get)
{
	if (!clever_check_no_args()) {
		return;
	}

	ArrayIteratorObject* iter_obj = clever_get_this(ArrayIteratorObject*);

	if (!iter_obj->isValid()) {
		result->setNull();
		return;
	}

	result->copy(*(iter_obj->getIterator()));
}

// Returns an iterator for the next position on the array
CLEVER_METHOD(ArrayIterator::next)
{
	if (!clever_check_no_args()) {
		return;
	}

	ArrayIteratorObject* iter_obj = clever_get_this(ArrayIteratorObject*);
	ArrayIteratorObject::InternalIteratorType it = iter_obj->getNext();

	result->setObj(this, new ArrayIteratorObject(iter_obj->getArray(), it));
}

// Operators

// == operator
CLEVER_TYPE_OPERATOR(ArrayIterator::equal)
{
	if (EXPECTED(rhs->getType() == this)) {
		ArrayIteratorObject* l_iter =
			static_cast<ArrayIteratorObject*>(lhs->getObj());
		ArrayIteratorObject* r_iter =
			static_cast<ArrayIteratorObject*>(rhs->getObj());

		result->setBool(l_iter->getIterator() == r_iter->getIterator());
	}
}

// != operator
CLEVER_TYPE_OPERATOR(ArrayIterator::not_equal)
{
	if (EXPECTED(rhs->getType() == this)) {
		ArrayIteratorObject* l_iter =
			static_cast<ArrayIteratorObject*>(lhs->getObj());
		ArrayIteratorObject* r_iter =
			static_cast<ArrayIteratorObject*>(rhs->getObj());

		result->setBool(l_iter->getIterator() != r_iter->getIterator());
	}
}

// Type initialization
CLEVER_TYPE_INIT(ArrayIterator::init)
{
	setConstructor((MethodPtr)&ArrayIterator::ctor);

	addMethod(new Function("get",  (MethodPtr)&ArrayIterator::get));
	addMethod(new Function("next", (MethodPtr)&ArrayIterator::next));
}

::std::string ArrayIterator::toString(TypeObject* value) const
{
	ArrayIteratorObject* iter = static_cast<ArrayIteratorObject*>(value);
	::std::ostringstream out;

	out << "<ArrayIterator: ";
	if (iter->isValid()) {
		out << (*(iter->getIterator()))->getType()->toString(
			(*(iter->getIterator()))->getObj());
	} else {
		out << "NULL";
	}
	out << ">";

	return out.str();
}

} // clever
