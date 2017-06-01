--TEST--
string php_ini_scanned_files ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--FILE--
<?php
var_dump(is_string(php_ini_scanned_files()));
?>
--EXPECT--
bool(true)
