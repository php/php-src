--TEST--
Changing LC_CTYPE back to C
--SKIPIF--
<?php
if (!setlocale(LC_CTYPE, "de_DE", "de-DE")) die("skip requires de_DE locale");
?>
--FILE--
<?php
var_dump(setlocale(LC_CTYPE, "de_DE", "de-DE") !== false);
var_dump(preg_match('/\w/', "\xe4"));
var_dump(setlocale(LC_CTYPE, "C") !== false);
var_dump(preg_match('/\w/', "\xe4"));
?>
--EXPECT--
bool(true)
int(1)
bool(true)
int(0)
