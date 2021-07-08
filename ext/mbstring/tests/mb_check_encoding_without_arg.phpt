--TEST--
Calling mb_check_encoding() without argument is deprecated
--EXTENSIONS--
mbstring
--FILE--
<?php

var_dump(mb_check_encoding());

?>
--EXPECTF--
Deprecated: mb_check_encoding(): Calling mb_check_encoding() without argument is deprecated in %s on line %d
bool(true)
