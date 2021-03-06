/**
 * Clever programming language
 * Copyright (c) Clever Team
 *
 * This file is distributed under the MIT license. See LICENSE for details.
 */

#ifndef CLEVER_TCPSOCKET_H
#define CLEVER_TCPSOCKET_H

#include <ostream>
#include "core/type.h"
#include "core/value.h"
#include "modules/std/net/csocket.h"

namespace clever { namespace modules { namespace std { namespace net {

class SocketObject : public TypeObject {
public:
	SocketObject() {}

	~SocketObject() {}

	CSocket& getSocket() { return m_socket; }
private:
	CSocket m_socket;

	DISALLOW_COPY_AND_ASSIGN(SocketObject);
};

class TcpSocket : public Type {
public:
	TcpSocket() :
		Type("TcpSocket") { }

	virtual void init();

	// Type methods
	CLEVER_METHOD(ctor);
	CLEVER_METHOD(setHost);
	CLEVER_METHOD(setPort);
	CLEVER_METHOD(setTimeout);
	CLEVER_METHOD(connect);
	CLEVER_METHOD(close);
	CLEVER_METHOD(receive);
	CLEVER_METHOD(send);
	CLEVER_METHOD(isOpen);
	CLEVER_METHOD(poll);
	CLEVER_METHOD(good);
	CLEVER_METHOD(getError);
	CLEVER_METHOD(getErrorMessage);
	CLEVER_METHOD(do_assign);
private:
	DISALLOW_COPY_AND_ASSIGN(TcpSocket);
};

}}}} // clever::modules::std::net

#endif // CLEVER_TCPSOCKET_H
