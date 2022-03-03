--TEST--
Bug #69267: mb_strtolower fails on titlecase characters
--EXTENSIONS--
mbstring
--FILE--
<?php

$str_l = "ǆǉǌǳ";
$str_u = "ǄǇǊǱ";
$str_t = "ǅǈǋǲ";
var_dump(mb_strtolower($str_l));
var_dump(mb_strtolower($str_u));
var_dump(mb_strtolower($str_t));
var_dump(mb_strtoupper($str_l));
var_dump(mb_strtoupper($str_u));
var_dump(mb_strtoupper($str_t));
var_dump(mb_convert_case($str_l, MB_CASE_TITLE));
var_dump(mb_convert_case($str_u, MB_CASE_TITLE));
var_dump(mb_convert_case($str_t, MB_CASE_TITLE));

$str_l = "ᾳ";
$str_t = "ᾼ";
var_dump(mb_strtolower($str_l));
var_dump(mb_strtolower($str_t));
var_dump(mb_strtoupper($str_l));
var_dump(mb_strtoupper($str_t));
var_dump(mb_convert_case($str_l, MB_CASE_TITLE));
var_dump(mb_convert_case($str_t, MB_CASE_TITLE));

?>
--EXPECT--
string(8) "ǆǉǌǳ"
string(8) "ǆǉǌǳ"
string(8) "ǆǉǌǳ"
string(8) "ǄǇǊǱ"
string(8) "ǄǇǊǱ"
string(8) "ǄǇǊǱ"
string(8) "ǅǉǌǳ"
string(8) "ǅǉǌǳ"
string(8) "ǅǉǌǳ"
string(3) "ᾳ"
string(3) "ᾳ"
string(4) "ΑΙ"
string(4) "ΑΙ"
string(3) "ᾼ"
string(3) "ᾼ"
