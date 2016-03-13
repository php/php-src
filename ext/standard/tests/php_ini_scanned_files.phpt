--TEST--
string php_ini_scanned_files ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.3.0") {
    die('SKIP php version so lower.');
}
?>
--FILE--
<?php
var_dump(is_string(php_ini_scanned_files()));
?>
--EXPECT--
bool(true)
