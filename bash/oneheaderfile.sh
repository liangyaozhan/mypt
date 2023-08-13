#!/usr/bin/bash

rm inone/include -rvf

mkdir inone/include/myptm -p

header=inone/include/myptm/myptm.h

cat <<- EOF >$header
/*
 * File: myptm_core.h
 * Author: Liang YaoZhan <262666882@qq.com>
 * Brief:  timer
 *
 * Copyright (c) 2022 - 2023 Liang YaoZhan <262666882@qq.com>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2023-08-05 Liang YaoZhan <262666882@qq.com> created
 *
 */

#ifndef MYPTM_H
#define MYPTM_H

#ifdef __cplusplus
extern "C"
{
#endif

EOF

includedir=../include/myptm
srcdir=../src

if [ ! -d $includedir ];  then 
    echo "dir $includedir not found"
    exit 3
fi

cat $includedir/myptm_queue.h | sed -e '/^#include "myptm.*/d' >>$header
cat $includedir/myptm_timer.h | sed -e '/^#include "myptm.*/d' >>$header
cat $includedir/myptm_core.h | sed -e '/^#include "myptm.*/d' >>$header
cat $includedir/myptm_message_queue.h | sed -e '/^#include "myptm.*/d' >>$header

cat <<- EOF >>$header

#ifdef __cplusplus
}
#endif

#endif /* header file once */

#ifdef DEF_MYPT_C_FUNCTION_BODY

#ifdef __cplusplus
extern "C"
{
#endif
EOF

cat $srcdir/myptm_core.c | sed -e '/^#include "myptm.*/d' >>$header
cat $srcdir/myptm_timer.c | sed -e '/^#include "myptm.*/d' >>$header
cat $srcdir/myptm_message_queue.c | sed -e '/^#include "myptm.*/d' >>$header

cat <<- EOF >>$header

#ifdef __cplusplus
}
#endif
#endif /* source file def: DEF_MYPT_C_FUNCTION_BODY */

#ifdef __cplusplus
EOF

cat $includedir/myptm.hpp | sed -e '/^#include "myptm.*/d' >>$header
cat <<- EOF >>$header
#endif /* source file def: DEF_MYPT_C_FUNCTION_BODY */
EOF

mkdir -p ../../allinone_example/include/myptm/
mv $header ../../allinone_example/include/myptm/
