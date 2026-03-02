--TEST--
INTL_ICU_VERSION constant
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(defined("INTL_ICU_VERSION"));
?>
--EXPECT--
bool(true)
