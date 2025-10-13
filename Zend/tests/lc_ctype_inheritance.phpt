--TEST--
Do not inherit LC_CTYPE from environment
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Darwin' && version_compare(php_uname('r'), '24.0.0', '>=')) {
    die('skip macOS 15 inherits LC_CTYPE into the thread locale');
}
if (!setlocale(LC_CTYPE, "de_DE", "de-DE")) die("skip requires de_DE locale");
?>
--ENV--
LC_CTYPE=de_DE
--FILE--
<?php

var_dump(setlocale(LC_CTYPE, "0"));
var_dump(bin2hex(strtoupper("\xe4")));
var_dump(preg_match('/\w/', "\xe4"));
var_dump(setlocale(LC_CTYPE, "de_DE", "de-DE") !== false);
var_dump(bin2hex(strtoupper("\xe4")));
var_dump(preg_match('/\w/', "\xe4"));
?>
--EXPECTF--
string(%d) "C%r(\.UTF-8)?%r"
string(2) "e4"
int(0)
bool(true)
string(2) "c4"
int(1)
