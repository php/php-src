--TEST--
string php_ini_scanned_files ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.3.0") {
    die('SKIP php version too low.');
}
?>
--FILE--
<?php
var_dump(is_string(php_ini_scanned_files()));
?>
--EXPECTF--
bool(%s)
