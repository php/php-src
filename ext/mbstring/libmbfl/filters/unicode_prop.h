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
 * The authors of this file: PHP3 Internationalization team
 *
 */

/* character property table */
#define MBFL_CHP_CTL		0x01
#define MBFL_CHP_DIGIT		0x02
#define MBFL_CHP_UALPHA		0x04
#define MBFL_CHP_LALPHA		0x08
#define MBFL_CHP_MMHQENC	0x10	/* must Q-encoding in MIME Header encoded-word */
#define MBFL_CHP_MSPECIAL	0x20	/* RFC822 Special characters */

/*
 * Unicode table
 */

static const unsigned char mbfl_charprop_table[] = {
/* NUL	0 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC,
/* SCH	1 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SIX	2 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* EIX	3 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* EOT	4 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* ENQ	5 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* ACK	6 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* BEL	7 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* BS	8 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* HI	9 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* LF	10 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* VI	11 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* FF	12 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* CR	13 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SO	14 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SI	15 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SLE	16 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* CSI	17 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* DC2	18 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* DC3	19 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* DC4	20 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* NAK	21 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SYN	22 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* EIB	23 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* CAN	24 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* EM	25 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SLB	26 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* ESC	27 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* FS	28 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* GS	29 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* RS	30 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* US	31 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SP	32 */	MBFL_CHP_MMHQENC ,
/* !	33 */	0 ,
/* "	34 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* #	35 */	MBFL_CHP_MMHQENC ,
/* $	36 */	MBFL_CHP_MMHQENC ,
/* %	37 */	MBFL_CHP_MMHQENC ,
/* &	38 */	MBFL_CHP_MMHQENC ,
/* '	39 */	MBFL_CHP_MMHQENC ,
/* (	40 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* )	41 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* *	42 */	0 ,
/* +	43 */	0 ,
/* ,	44 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* -	45 */	0 ,
/* .	46 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* /	47 */	0 ,
/* 0	48 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 1	49 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 2	50 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 3	51 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 4	52 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 5	53 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 6	54 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 7	55 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 8	56 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 9	57 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* :	58 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* ;	59 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* <	60 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* =	61 */	0 ,
/* >	62 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* ?	63 */	MBFL_CHP_MMHQENC ,
/* @	64 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* A	65 */	MBFL_CHP_UALPHA ,
/* B	66 */	MBFL_CHP_UALPHA ,
/* C	67 */	MBFL_CHP_UALPHA ,
/* D	68 */	MBFL_CHP_UALPHA ,
/* E	69 */	MBFL_CHP_UALPHA ,
/* F	70 */	MBFL_CHP_UALPHA ,
/* G	71 */	MBFL_CHP_UALPHA ,
/* H	72 */	MBFL_CHP_UALPHA ,
/* I	73 */	MBFL_CHP_UALPHA ,
/* J	74 */	MBFL_CHP_UALPHA ,
/* K	75 */	MBFL_CHP_UALPHA ,
/* L	76 */	MBFL_CHP_UALPHA ,
/* M	77 */	MBFL_CHP_UALPHA ,
/* N	78 */	MBFL_CHP_UALPHA ,
/* O	79 */	MBFL_CHP_UALPHA ,
/* P	80 */	MBFL_CHP_UALPHA ,
/* Q	81 */	MBFL_CHP_UALPHA ,
/* R	82 */	MBFL_CHP_UALPHA ,
/* S	83 */	MBFL_CHP_UALPHA ,
/* T	84 */	MBFL_CHP_UALPHA ,
/* U	85 */	MBFL_CHP_UALPHA ,
/* V	86 */	MBFL_CHP_UALPHA ,
/* W	87 */	MBFL_CHP_UALPHA ,
/* X	88 */	MBFL_CHP_UALPHA ,
/* Y	89 */	MBFL_CHP_UALPHA ,
/* Z	90 */	MBFL_CHP_UALPHA ,
/* [	91 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* \	92 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* ]	93 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* ^	94 */	MBFL_CHP_MMHQENC ,
/* _	95 */	MBFL_CHP_MMHQENC ,
/* `	96 */	MBFL_CHP_MMHQENC ,
/* a	97 */	MBFL_CHP_LALPHA ,
/* b	98 */	MBFL_CHP_LALPHA ,
/* c	99 */	MBFL_CHP_LALPHA ,
/* d	100 */	MBFL_CHP_LALPHA ,
/* e	101 */	MBFL_CHP_LALPHA ,
/* f	102 */	MBFL_CHP_LALPHA ,
/* g	103 */	MBFL_CHP_LALPHA ,
/* h	104 */	MBFL_CHP_LALPHA ,
/* i	105 */	MBFL_CHP_LALPHA ,
/* j	106 */	MBFL_CHP_LALPHA ,
/* k	107 */	MBFL_CHP_LALPHA ,
/* l	108 */	MBFL_CHP_LALPHA ,
/* m	109 */	MBFL_CHP_LALPHA ,
/* n	110 */	MBFL_CHP_LALPHA ,
/* o	111 */	MBFL_CHP_LALPHA ,
/* p	112 */	MBFL_CHP_LALPHA ,
/* q	113 */	MBFL_CHP_LALPHA ,
/* r	114 */	MBFL_CHP_LALPHA ,
/* s	115 */	MBFL_CHP_LALPHA ,
/* t	116 */	MBFL_CHP_LALPHA ,
/* u	117 */	MBFL_CHP_LALPHA ,
/* v	118 */	MBFL_CHP_LALPHA ,
/* w	119 */	MBFL_CHP_LALPHA ,
/* x	120 */	MBFL_CHP_LALPHA ,
/* y	121 */	MBFL_CHP_LALPHA ,
/* z	122 */	MBFL_CHP_LALPHA ,
/* {	123 */	MBFL_CHP_MMHQENC ,
/* |	124 */	MBFL_CHP_MMHQENC ,
/* }	125 */	MBFL_CHP_MMHQENC ,
/* ~	126 */	MBFL_CHP_MMHQENC ,
/* DEL	127 */	MBFL_CHP_MMHQENC
};

