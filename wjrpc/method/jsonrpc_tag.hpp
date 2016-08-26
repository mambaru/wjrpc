//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2011-2016
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <fas/functional/tstring.hpp>

#define JSONRPC_TAG(x) FAS_STRING( _##x##_, #x)
#define JSONRPC_TAG2(x,y) FAS_STRING( x, y)

