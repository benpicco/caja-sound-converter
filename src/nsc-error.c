/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 *  nsc-error.c
 * 
 *  Copyright (C) 2008 Brian Pepple
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Author: Brian Pepple <bpepple@fedoraproject.org>
 * 
 */

#include <config.h>

#include "nsc-error.h"

GQuark
nsc_error_quark (void)
{
	static GQuark q = 0;

	if (q == 0) {
		q = g_quark_from_static_string ("nsc-error-quark");
	}

	return q;
}
