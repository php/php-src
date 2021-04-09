--TEST--
Bug #77594 (ob_tidyhandler is never reset)
--DESCRIPTION--
Test is useful only with --repeat 2 (or more)
--EXTENSIONS--
tidy
--FILE--
<?php
ob_start('ob_tidyhandler');
var_dump(ob_end_clean());
?>
--EXPECT--
bool(true)
