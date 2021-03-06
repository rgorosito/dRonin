/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, see <http://www.gnu.org/licenses/>

 *****************************************************************************/
//! \file glc_contextshareddata.cpp implementation of the GLC_ContextSharedData class.

#include <QtDebug>

#include "glc_contextshareddata.h"

GLC_ContextSharedData::GLC_ContextSharedData()
{
	qDebug() << "GLC_ContextSharedData::GLC_ContextSharedData()";

}

GLC_ContextSharedData::~GLC_ContextSharedData()
{
	qDebug() << "GLC_ContextSharedData::~GLC_ContextSharedData()";
}
