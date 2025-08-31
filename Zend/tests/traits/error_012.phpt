--TEST--
Trying to access a protected trait method
--FILE--
<?php

trait foo {
    public function test() { return 3; }
}

class bar {
    use foo { test as protected; }
}

$x = new bar;
var_dump($x->test());

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to protected method bar::test() from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
