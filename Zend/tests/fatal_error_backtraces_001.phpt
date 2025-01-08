--TEST--
Fatal error backtrace
--FILE--
<?php

ini_set('fatal_error_backtraces', true);

eval("class Foo {}; class Foo {}");
?>
--EXPECTF--
Fatal error: Cannot redeclare class Foo (%s) in %s : eval()'d code on line %d
Stack trace:
#0 %sfatal_error_backtraces_001.php(%d): eval()
#1 {main}
