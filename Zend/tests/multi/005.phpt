--TEST--
multi type mix
--FILE--
<?php
function(callable | array & Foo $arg) {};
?>
--EXPECTF--
Fatal error: Cannot use intersection when creating union type in %s on line 2
