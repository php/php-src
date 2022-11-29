--TEST--
Do not inherit LC_CTYPE from environment
--EXTENSIONS--
ctype
--SKIPIF--
<?php
if (setlocale(LC_ALL, 'invalid') === 'invalid') { die('skip setlocale() is broken /w musl'); }
if (!setlocale(LC_CTYPE, "de_DE", "de-DE")) die("skip requires de_DE locale");
?>
--ENV--
LC_CTYPE=de_DE
--FILE--
<?php

echo "inherited\n";
echo 'ctype_lower(\xe4): ' . (ctype_lower("\xe4") ? 'y' : 'n') . "\n";
echo 'preg_match(\w, \xe4): ';
var_dump(preg_match('/\w/', "\xe4"));

var_dump(setlocale(LC_CTYPE, "0"));
echo 'ctype_lower(\xe4): ' . (ctype_lower("\xe4") ? 'y' : 'n') . "\n";
echo 'preg_match(\w, \xe4): ';
var_dump(preg_match('/\w/', "\xe4"));

echo "de_DE\n";
var_dump(setlocale(LC_CTYPE, "de_DE", "de-DE") !== false);
echo 'ctype_lower(\xe4): ' . (ctype_lower("\xe4") ? 'y' : 'n') . "\n";
echo 'preg_match(\w, \xe4): ';
var_dump(preg_match('/\w/', "\xe4"));
?>
--EXPECTF--
inherited
ctype_lower(\xe4): n
preg_match(\w, \xe4): int(0)
string(%d) "C%r(\.UTF-8)?%r"
ctype_lower(\xe4): n
preg_match(\w, \xe4): int(0)
de_DE
bool(true)
ctype_lower(\xe4): y
preg_match(\w, \xe4): int(1)
