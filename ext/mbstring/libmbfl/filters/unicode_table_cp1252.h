/*
 * "streamable kanji code filter and converter"
 * Copyright (c) 1998-2002 HappySize, Inc. All rights reserved.
 *
 * LICENSE NOTICES
 *
 * This file is part of "streamable kanji code filter and converter",
 * which is distributed under the terms of GNU Lesser General Public
 * License (version 2) as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with "streamable kanji code filter and converter";
 * if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307  USA
 *
 * The authors of this file: PHP3 internationalization team
 * You can contact the primary author 金本　茂 <sgk@happysize.co.jp>.
 *
 */

#ifndef UNICODE_TABLE_CP1252_H
#define UNICODE_TABLE_CP1252_H

/* Windows CodePage 1252 - it's the same as iso-8859-1 but
 * defines extra symbols in the range 0x80-0x9f.
 * This table differs from the rest of the unicode tables below
 * as it only covers this range, while the rest cover 0xa0 onwards */

static const unsigned short cp1252_ucs_table[] = {
 0x20ac,0xfffd,0x201a,0x0192,0x201e,0x2026,0x2020,0x2021,
 0x02c6,0x2030,0x0160,0x2039,0x0152,0xfffd,0x017d,0xfffd,
 0xfffd,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,
 0x02dc,0x2122,0x0161,0x203a,0x0153,0xfffd,0x017e,0x0178
};
#endif /* UNICODE_TABLE_CP1252_H */
