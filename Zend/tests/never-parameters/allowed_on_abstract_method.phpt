--TEST--
`never` parameter types - allowed on abstract methods
--FILE--
<?php

abstract class Demo {
    abstract public function example(never $v);
}

?>
--EXPECT--
