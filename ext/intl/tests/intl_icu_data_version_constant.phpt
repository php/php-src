--TEST--
INTL_ICU_DATA_VERSION constant
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip intl extension not loaded'; ?>
--FILE--
<?php
var_dump(defined("INTL_ICU_DATA_VERSION"));
?>
--EXPECT--
bool(true)
