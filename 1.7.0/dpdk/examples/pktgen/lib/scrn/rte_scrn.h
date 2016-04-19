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

#ifndef __INC_RTE_SCRN_H
#define __INC_RTE_SCRN_H

#include <string.h>
#include <stdio.h>

#include <rte_atomic.h>

#define SCRN_VERSION	"1.2.0"

typedef struct rte_scrn_s {
	rte_atomic32_t	pause;		//!< Pause the update of the screen.
	rte_atomic32_t	state;		//!< Screen state on or off
	uint16_t		nrows;		//!< Max number of rows.
	uint16_t		ncols;		//!< Max number of columns.
	uint16_t		theme;		//!< Current theme state on or off
	uint16_t		pad0;
} rte_scrn_t;

enum { SCRN_ON = 0, SCRN_OFF = 1 };
enum { THEME_OFF = 0, THEME_ON = 1 };

typedef enum { BLACK = 0, RED = 1, GREEN = 2, YELLOW = 3, BLUE = 4, MAGENTA = 5, CYAN = 6, WHITE = 7,
			   RGB = 8, DEFAULT_FG = 9, DEFAULT_BG = 9, NO_CHANGE = 98, UNKNOWN_COLOR=99 } color_e;
typedef enum { OFF = 0, BOLD = 1, UNDERSCORE = 4, BLINK = 5, REVERSE = 7, CONCEALED = 8, NO_ATTR = 98, UNKNOWN_ATTR = 99 } attr_e;

typedef uint8_t		rgb_t;

extern rte_scrn_t * scrn;

#define	scrn_puts(...)	{ printf(__VA_ARGS__); fflush(stdout); }

static __inline__ void scrn_pos(int r, int c)	scrn_puts("\033[%d;%dH", r, c)
static __inline__ void scrn_top(void)			scrn_puts("\033H")
static __inline__ void scrn_home(void)			scrn_puts("\033H")
static __inline__ void scrn_coff(void)			scrn_puts("\033[?25l")
static __inline__ void scrn_con(void)			scrn_puts("\033[?25h")
static __inline__ void scrn_save(void)			scrn_puts("\0337")
static __inline__ void scrn_restore(void)		scrn_puts("\0338")
static __inline__ void scrn_cls(void)			scrn_puts("\033[2J")
static __inline__ void scrn_eol(void)			scrn_puts("\033[K")
static __inline__ void scrn_cel(void)			scrn_puts("\033[2K")
static __inline__ void scrn_clw(void)			scrn_puts("\033[J")
static __inline__ void scrn_reverse(void)		scrn_puts("\033[7m")
static __inline__ void scrn_normal(void)		scrn_puts("\033[0m")
static __inline__ void scrn_scroll(int r)		scrn_puts("\033[%d;r",r)
static __inline__ void scrn_scroll_up(int r)	scrn_puts("\033[%dS",r)
static __inline__ void scrn_scroll_down(int r)	scrn_puts("\033[%dT",r)
static __inline__ void scrn_nlines(int r)		scrn_puts("\033[%dE",r)
static __inline__ void scrn_turn_on(void)		scrn_puts("\033[?25h")
static __inline__ void scrn_turn_off(void)		scrn_puts("\033[?25l")
static __inline__ void scrn_setw(int t)			scrn_puts("\033[%d;r", t)
static __inline__ void scrn_cpos(void)			scrn_puts("\0336n")

static __inline__ const char * scrn_version(void) {
	return SCRN_VERSION;
}

static __inline__ void scrn_clr_line(int r)	{
	scrn_pos(r, 0);
	scrn_cel();
}

static __inline__ void scrn_eol_pos(int r, int c) {
	scrn_pos(r, c);
	scrn_eol();
}

static __inline__ void scrn_off(void) {
	if ( scrn == NULL ) return;
	rte_atomic32_set(&scrn->state, SCRN_OFF);
}

static __inline__ void scrn_on(void) {
	if ( scrn == NULL ) return;
	rte_atomic32_set(&scrn->state, SCRN_ON);
}

static __inline__ int scrn_is_off(void) {
	if ( scrn == NULL ) return 0;
	return (rte_atomic32_read(&scrn->state) == SCRN_OFF);
}

static __inline__ void scrn_pause(void) {
	if ( scrn == NULL ) return;
	rte_atomic32_set(&scrn->pause, 1);
}

static __inline__ void scrn_resume(void) {
	if ( scrn == NULL ) return;
	rte_atomic32_set(&scrn->pause, 0);
}

static __inline__ int scrn_is_paused(void) {
	if ( scrn == NULL ) return 0;
	return (rte_atomic32_read(&scrn->pause) == 1);
}

static __inline__ int scrn_center_col(const char * msg) {
	int16_t		s;

	if ( scrn == NULL ) return 0;
	s = ((scrn->ncols/2) - (strlen(msg)/2));
	return (s <= 0)? 1 : s;
}

static __inline__ void scrn_erase(void) {
	int		i;

	scrn_setw(1);				// Clear the window to full screen.
	scrn_pos(100, 1);			// Put cursor on the last row.

	// Scroll the screen to clear the screen and keep the previous information.
	for(i = 0; i < ((scrn->nrows+5)/6); i++)
		printf("\n\n\n\n\n\n");
	fflush(stdout);
}

static __inline__ void scrn_repeat(int16_t r, int16_t c, const char * str, int cnt) {
	int		i;

	scrn_pos(r, c);
	for(i=0; i<cnt; i++)
		printf("%s", str);

	fflush(stdout);
}

static __inline__ void scrn_col_repeat(int16_t r, int16_t c, const char * str, int cnt) {
	int		i;

	for(i=0; i<cnt; i++) {
		scrn_pos(r++, c);
		printf("%s", str);
	}
}

static __inline__ void scrn_fgcolor( color_e color, attr_e attr ) {
    scrn_puts("\033[%d;%dm", attr, color + 30);
}

static __inline__ void scrn_bgcolor( color_e color, attr_e attr ) {
    scrn_puts("\033[%d;%dm", attr, color + 40);
}

static __inline__ void scrn_fgbgcolor( color_e fg, color_e bg, attr_e attr ) {
		scrn_puts("\033[%d;%d;%dm", attr, fg + 30, bg + 40);
}

static __inline__ void scrn_color(color_e fg, color_e bg, attr_e attr) {

	if ( (fg != NO_CHANGE) && (bg != NO_CHANGE) )
		scrn_fgbgcolor(fg, bg, attr);
	else if ( fg == NO_CHANGE )
		scrn_bgcolor(bg, attr);
	else if ( bg == NO_CHANGE )
		scrn_fgcolor(fg, attr);
}

// Setup for 256 RGB color methods
static __inline__ void scrn_rgb(uint8_t fg_bg, rgb_t r, rgb_t g, rgb_t b) {
	scrn_puts("\033[%d;2;%d;%d;%dm", fg_bg, r, g, b);
}

extern void scrn_center(int16_t r, const char * fmt, ...);
extern void scrn_printf(int16_t r, int16_t c, const char * fmt, ...);
extern void scrn_fprintf(int16_t r, int16_t c, FILE * f, const char * fmt, ...);
extern void scrn_snprintf(char * buff, int16_t len, const char * fmt, ...);
extern rte_scrn_t * scrn_init(int16_t nrows, int16_t ncols, int theme);

#define printf_status(...)	scrn_fprintf(0, 0, stdout, __VA_ARGS__)
#define printf_info(...)	scrn_fprintf(0, 0, stdout, __VA_ARGS__)

#endif
