/**
 * Clever programming language
 * Copyright (c) Clever Team
 *
 * This file is distributed under the MIT license. See LICENSE for details.
 */

#ifndef CLEVER_USER_H
#define CLEVER_USER_H

namespace clever {

// User object representation
class UserObject {
public:
	UserObject() {}
	~UserObject() {}
};

// User type representation
class UserType : public Type {
public:
	UserType(const CString* name)
		: Type(name) {}

	~UserType() {}

	void* allocData(CLEVER_TYPE_CTOR_ARGS) const { return new UserObject; }

	void deallocData(CLEVER_TYPE_DTOR_ARGS) { delete static_cast<UserObject*>(data); }

	virtual void dump(const void*) const {}
	virtual void dump(const void*, std::ostream& out) const {}

	void init(CLEVER_TYPE_INIT_ARGS) {
		Function* ctor = new Function(*getName(), (MethodPtr) &UserType::ctor);
		setConstructor(ctor);
		addMethod(ctor);
	}

	CLEVER_METHOD(ctor) {
		result->setObj(this, allocData(&args));
	}
};

// User module representation
class UserModule: public Module {
public:
	UserModule()
		: Module("_user") {}

	~UserModule() {}

	void init() {}
};

} // clever

#endif // CLEVER_USER_H