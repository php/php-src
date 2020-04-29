--TEST--
Do not inherit LC_CTYPE from environment
--SKIPIF--
<?php
if (!setlocale(LC_CTYPE, "de_DE")) die("skip requires de_DE locale");
?>
--ENV--
LC_CTYPE=de_DE
--FILE--
<?php

var_dump(bin2hex(strtoupper("\xe4")));
var_dump(preg_match('/\w/', "\xe4"));
var_dump(setlocale(LC_ALL, "de_DE"));
var_dump(bin2hex(strtoupper("\xe4")));
var_dump(preg_match('/\w/', "\xe4"));
?>
--EXPECT--
string(2) "e4"
int(0)
string(5) "de_DE"
string(2) "c4"
int(1)
