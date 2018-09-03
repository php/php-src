--TEST--
INTL_ICU_DATA_VERSION constant
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip intl extension not loaded'; ?>
<?php if(version_compare('INTL_ICU_VERSION', '4.4', '<') < 0) print 'skip for ICU >= 4.4'; ?>
--FILE--
<?php
var_dump(defined("INTL_ICU_DATA_VERSION"));
?>
--EXPECT--
bool(true)
