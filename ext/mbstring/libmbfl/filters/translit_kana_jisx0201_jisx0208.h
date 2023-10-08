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
 * The author of this file: Moriyoshi Koizumi <koizumi@gree.co.jp>
 *
 */

#ifndef TRANSLIT_KANA_JISX0201_JISX0208_H
#define TRANSLIT_KANA_JISX0201_JISX0208_H

/* "Zen" is 全, or "full"; "Han" is 半, or "half"
 * This refers to "fullwidth" or "halfwidth" variants of characters used for writing Japanese */
#define MBFL_HAN2ZEN_ALL       0x00001
#define MBFL_HAN2ZEN_ALPHA     0x00002
#define MBFL_HAN2ZEN_NUMERIC   0x00004
#define MBFL_HAN2ZEN_SPACE     0x00008
#define MBFL_HAN2ZEN_KATAKANA  0x00010
#define MBFL_HAN2ZEN_HIRAGANA  0x00020
#define MBFL_HAN2ZEN_SPECIAL   0x00040
#define MBFL_ZENKAKU_HIRA2KATA 0x00080

#define MBFL_ZEN2HAN_ALL       0x00100
#define MBFL_ZEN2HAN_ALPHA     0x00200
#define MBFL_ZEN2HAN_NUMERIC   0x00400
#define MBFL_ZEN2HAN_SPACE     0x00800
#define MBFL_ZEN2HAN_KATAKANA  0x01000
#define MBFL_ZEN2HAN_HIRAGANA  0x02000
#define MBFL_ZEN2HAN_SPECIAL   0x04000
#define MBFL_ZENKAKU_KATA2HIRA 0x08000

#define MBFL_HAN2ZEN_GLUE      0x10000

#endif /* TRANSLIT_KANA_JISX0201_JISX0208_H */
