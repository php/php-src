--TEST--
mb_convert_encoding() when encoding detection fails
--FILE--
<?php

var_dump(mb_convert_encoding("\xff", "ASCII", ["UTF-8", "UTF-16"]));

?>
--EXPECTF--
Warning: mb_convert_encoding(): Unable to detect character encoding in %s on line %d
bool(false)
