--TEST--
multi type repeat
--FILE--
<?php
function(callable or callable $arg) {};
?>
--EXPECTF--
Fatal error: callable is already present in union in %s on line 2
