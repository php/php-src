--TEST--
GH-20674 (mb_decode_mimeheader does not handle separator)
--EXTENSIONS--
mbstring
--FILE--
<?php

$subject = '=?us-ascii?Q?The_PH?=  =?us-ascii?Q?P_8.5?=';
var_dump(mb_decode_mimeheader($subject));

// mb_decode_mimeheader's backward compatible for TAB(\t)
$subject = "=?us-ascii?Q?The_PH?=\t=?us-ascii?Q?P_8.5?=";
var_dump(mb_decode_mimeheader($subject));

$subject = "=?us-ascii?Q?The_PH?=\t =?us-ascii?Q?P_8.5?=";
var_dump(mb_decode_mimeheader($subject));

$subject = "=?us-ascii?Q?The_PH?= \t =?us-ascii?Q?P_8.5?=";
var_dump(mb_decode_mimeheader($subject));

// from RFC 2047 https://www.ietf.org/rfc/rfc2047#section-8
var_dump(mb_decode_mimeheader("(=?ISO-8859-1?Q?a?=)"));
var_dump(mb_decode_mimeheader("(=?ISO-8859-1?Q?a?= b)"));
var_dump(mb_decode_mimeheader("(=?ISO-8859-1?Q?a_b?=)"));
var_dump(mb_decode_mimeheader("(=?ISO-8859-1?Q?a?= =?ISO-8859-1?Q?b?=)"));
var_dump(mb_decode_mimeheader("(=?ISO-8859-1?Q?a?=  =?ISO-8859-1?Q?b?=)"));
var_dump(mb_decode_mimeheader("(=?ISO-8859-1?Q?a?=
	=?ISO-8859-1?Q?b?=)"));
?>
--EXPECTF--
string(11) "The PHP 8.5"
string(11) "The PHP 8.5"
string(11) "The PHP 8.5"
string(11) "The PHP 8.5"
string(3) "(a)"
string(5) "(a b)"
string(5) "(a b)"
string(4) "(ab)"
string(4) "(ab)"
string(4) "(ab)"
