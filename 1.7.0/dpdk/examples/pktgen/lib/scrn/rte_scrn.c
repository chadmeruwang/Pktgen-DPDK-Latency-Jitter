/*-
 * Copyright (c) <2010>, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * - Neither the name of Intel Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Copyright (c) <2010-2014>, Wind River Systems, Inc.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1) Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3) Neither the name of Wind River Systems nor the names of its contributors may be
 * used to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * 4) The screens displayed by the application must contain the copyright notice as defined
 * above and can not be removed without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Created 2010 by Keith Wiles @ windriver.com */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include <rte_config.h>
#include <rte_atomic.h>
#include <rte_malloc.h>
#include <rte_spinlock.h>

#include "rte_scrn.h"

	rte_scrn_t * scrn;

void scrn_center(int16_t r, const char * fmt, ...) {
	va_list	vaList;
	char	str[256];

	va_start(vaList, fmt);
	vsnprintf(str, sizeof(str), fmt, vaList);
	va_end(vaList);
	scrn_pos(r, scrn_center_col(str));
	printf("%s", str);
	fflush(stdout);
}

void scrn_printf(int16_t r, int16_t c, const char * fmt, ...) {
	va_list	vaList;
	
	if ( (r != 0) && (c != 0) )
		scrn_pos(r, c);
	va_start(vaList, fmt);
	vprintf(fmt, vaList);
	va_end(vaList);
	fflush(stdout);
}

void scrn_snprintf(char * buff, int16_t len, const char * fmt, ...) {
	va_list	vaList;
	
	va_start(vaList, fmt);
	vsnprintf(buff, len, fmt, vaList);
	va_end(vaList);
	fflush(stdout);
}

void scrn_fprintf(int16_t r, int16_t c, FILE * f, const char * fmt, ...) {
	va_list	vaList;

	if ( (r != 0) && (c != 0) )
		scrn_pos(r, c);
	va_start(vaList, fmt);
	vfprintf(f, fmt, vaList);
	va_end(vaList);
	fflush(f);
}

rte_scrn_t * scrn_init(int16_t nrows, int16_t ncols, int theme)
{
	scrn = malloc(sizeof(rte_scrn_t));
	if ( scrn == NULL )
		return NULL;
	
	rte_atomic32_set(&scrn->state, SCRN_ON);
	rte_atomic32_set(&scrn->pause, 1);

	scrn->nrows		= nrows;
	scrn->ncols		= ncols;
	scrn->theme		= theme;
	scrn_color(DEFAULT_FG, DEFAULT_BG, OFF);

	scrn_erase();

	return scrn;
}
