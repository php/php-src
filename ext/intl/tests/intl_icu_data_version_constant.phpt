--TEST--
INTL_ICU_DATA_VERSION constant
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(defined("INTL_ICU_DATA_VERSION"));
?>
--EXPECT--
bool(true)
