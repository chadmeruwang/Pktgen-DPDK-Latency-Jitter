/*-
 * Copyright (c) <2010-2014>, Intel Corporation
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
/* Created 2013 by Keith Wiles @ windriver.com */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include <rte_version.h>
#include <rte_config.h>
#include <rte_atomic.h>
#include <rte_cycles.h>

#include "rte_scrn.h"
#include "wr_copyright_info.h"

#define COPYRIGHT_MSG			"Copyright (c) <2010-2014>, Wind River Systems, Inc."
#define POWERED_BY_DPDK			"Powered by Intel® DPDK"

static const char * intel_copyright[] = {
	"",
	"   BSD LICENSE",
	"",
	"   Copyright(c) 2010-2014 Intel Corporation. All rights reserved.",
	"   All rights reserved.",
	"",
	"   Redistribution and use in source and binary forms, with or without",
	"   modification, are permitted provided that the following conditions",
	"   are met:",
	"",
	"     * Redistributions of source code must retain the above copyright",
	"       notice, this list of conditions and the following disclaimer.",
	"     * Redistributions in binary form must reproduce the above copyright",
	"       notice, this list of conditions and the following disclaimer in",
	"       the documentation and/or other materials provided with the",
	"       distribution.",
	"     * Neither the name of Intel Corporation nor the names of its",
	"       contributors may be used to endorse or promote products derived",
	"       from this software without specific prior written permission.",
	"",
	"   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS",
	"   \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT",
	"   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR",
	"   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT",
	"   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,",
	"   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT",
	"   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,",
	"   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY",
	"   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT",
	"   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE",
	"   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.",
	"",
	NULL
};
static const char * wr_copyright[] = {
	""
	"   Redistribution and use in source and binary forms, with or without modification, are",
	"   permitted provided that the following conditions are met:",
	"",
	"     1) Redistributions of source code must retain the above copyright notice,",
	"        this list of conditions and the following disclaimer.",
	"",
	"     2) Redistributions in binary form must reproduce the above copyright notice,",
	"        this list of conditions and the following disclaimer in the documentation and/or",
	"        other materials provided with the distribution.",
	"",
	"     3) Neither the name of Wind River Systems nor the names of its contributors may be",
	"        used to endorse or promote products derived from this software without specific",
	"        prior written permission.",
	"",
	"     4) The screens displayed by the application must contain the copyright notice as defined",
	"        above and can not be removed without specific prior written permission.",
	"",
	"   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"",
	"   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE",
	"   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE",
	"   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE",
	"   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL",
	"   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR",
	"   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER",
	"   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,",
	"   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE",
	"   USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.",
	"",
	NULL
};

/**************************************************************************//**
*
* wr_print_copyright - Print out the copyright notices.
*
* DESCRIPTION
* Output the copyright notices.
*
* RETURNS: N/A
*
* SEE ALSO:
*/

void
wr_print_copyright(char * appname, char * created_by)
{
	int		i;

	printf_info("-----------------------\n");
	for(i=0; intel_copyright[i] != NULL; i++)
		printf_info("  %s\n", intel_copyright[i]);
	printf_info("-----------------------\n");

	printf_info("    %s\n\n", COPYRIGHT_MSG);
	for(i=0; wr_copyright[i] != NULL; i++)
		printf_info("  %s\n", wr_copyright[i]);

	scrn_color(YELLOW, NO_CHANGE, OFF);
	printf_info("  %s created by: %s -- >>> %s <<<\n", appname, created_by, POWERED_BY_DPDK);
	scrn_color(BLUE, NO_CHANGE, OFF);
	printf_info("-----------------------\n");
}

void
wr_logo(int row, int col, char * appname)
{
	int		i;
	static const char * logo[] = {
		"#     #",
		"#  #  #     #    #    #  #####",
		"#  #  #     #    ##   #  #    #",
		"#  #  #     #    # #  #  #    #",
		"#  #  #     #    #  # #  #    #",
		"#  #  #     #    #   ##  #    #",
		" ## ##      #    #    #  #####",
		"",
		"######",
		"#     #     #    #    #  ######  #####",
		"#     #     #    #    #  #       #    #",
		"######      #    #    #  #####   #    #",
		"#   #       #    #    #  #       #####",
		"#    #      #     #  #   #       #   #",
		"#     #     #      ##    ######  #    #",
		"",
		" #####",
		"#     #   #   #   ####    #####  ######  #    #   ####",
		"#          # #   #          #    #       ##  ##  #",
		" #####      #     ####      #    #####   # ## #   ####",
		"      #     #         #     #    #       #    #       #",
		"#     #     #    #    #     #    #       #    #  #    #",
		" #####      #     ####      #    ######  #    #   ####",
		NULL
	};

	scrn_cls();
	scrn_color(GREEN, NO_CHANGE, BOLD);
	for(i=0, row++; logo[i] != NULL; i++)
		scrn_printf(row++, 7, "%s", logo[i]);

	scrn_color(MAGENTA, NO_CHANGE, OFF);
	scrn_printf(++row, col, "%s", COPYRIGHT_MSG);
	scrn_color(BLUE, NO_CHANGE, BOLD);
	scrn_printf(++row, col+6, ">>> %s is %s <<<", appname, POWERED_BY_DPDK);
	scrn_color(BLACK, NO_CHANGE, OFF);
	scrn_pos(++row, 1);

	rte_delay_ms(1500);

    scrn_cls();
    scrn_pos(100, 1);
}

void
wr_splash_screen(int row, int col, char * appname, char * created_by)
{
	int		i;

	row = 3;
	scrn_color(BLUE, NO_CHANGE, OFF);
	scrn_printf(row++, col, "%s", COPYRIGHT_MSG);
	scrn_color(GREEN, NO_CHANGE, BOLD);
	for(i=0, row++; wr_copyright[i] != NULL; i++)
		scrn_printf(row++, 7, "%s", wr_copyright[i]);
	scrn_color(BLUE, NO_CHANGE, BOLD);
	scrn_printf(row++, col, "%s created by %s -- >>> %s <<<", appname, created_by, POWERED_BY_DPDK);
	scrn_color(BLACK, NO_CHANGE, OFF);
	scrn_pos(++row, 1);

	rte_delay_ms(1500);

    scrn_cls();
    scrn_pos(100, 1);
}

/**
 * Function returning string for Copyright message."
 * @return
 *     string
 */
const char *
wr_copyright_msg(void) {
	return COPYRIGHT_MSG;
}

/**
 * Function returning string for Copyright message."
 * @return
 *     string
 */
const char *
wr_powered_by(void) {
	return POWERED_BY_DPDK;
}

