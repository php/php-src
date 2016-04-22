--TEST--
multi type repeat complex
--FILE--
<?php
function(Foo | Foo $arg) {};
?>
--EXPECTF--
Fatal error: Foo is already present in union in %s on line 2
