--TEST--
Return type of self is allowed in closure

--FILE--
<?php

$c = function(): self { return $this; };
var_dump($c->call(new stdClass));

--EXPECT--
object(stdClass)#2 (0) {
}
