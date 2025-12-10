--TEST--
GH-20674 (mb_decode_mimeheader does not handle separator)
--EXTENSIONS--
mbstring
--FILE--
<?php

$subject = '=?us-ascii?Q?The_PH?=  =?us-ascii?Q?P_8.5?=';
var_dump(mb_decode_mimeheader($subject));
?>
--EXPECTF--
string(11) "The PHP 8.5"
