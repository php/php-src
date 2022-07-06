--TEST--
Bug #77594 (ob_tidyhandler is never reset)
--DESCRIPTION--
Test is useful only with --repeat 2 (or more)
--SKIPIF--
<?php
if (!extension_loaded('tidy')) die('skip tidy extension not available');
?>
--FILE--
<?php
ob_start('ob_tidyhandler');
var_dump(ob_end_clean());
?>
--EXPECT--
bool(true)
