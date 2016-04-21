--TEST--
multi type mix
--FILE--
<?php
function(callable or array and Foo $arg) {};
?>
--EXPECTF--
Fatal error: Cannot use intersection when creating union type in %s on line 2
