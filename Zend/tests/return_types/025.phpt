--TEST--
Return type of self is allowed in closure
--FILE--
<?php

$c = function(): self { return $this; };
class Bar { }
var_dump($c->call(new Bar));
?>
--EXPECT--
object(Bar)#2 (0) {
}
