--TEST--
string zlib_get_coding_type ( void );
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
var_dump(zlib_get_coding_type());
?>
--EXPECT--
bool(false)
