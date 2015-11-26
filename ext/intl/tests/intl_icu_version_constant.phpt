--TEST--
INTL_ICU_VERSION constant
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
var_dump(defined("INTL_ICU_VERSION"));
?>
--EXPECT--
bool(true)
