--TEST--
Cleanup of basic block kept only because of FREE loop var
--FILE--
<?php
var_dump(X||match(X and true or true){false=>X});
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "X" in %smatch_scdf_cleanup.php:2
Stack trace:
#0 {main}
  thrown in %smatch_scdf_cleanup.php on line 2
