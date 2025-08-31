--TEST--
Testing 'self', 'parent' as type-hint
--FILE--
<?php

interface iTest { }

class baz implements iTest {}

class bar { }

class foo extends bar {
    public function testFoo(self $obj) {
        var_dump($obj);
    }
    public function testBar(parent $obj) {
        var_dump($obj);
    }
    public function testBaz(iTest $obj) {
        var_dump($obj);
    }
}

$foo = new foo;
$foo->testFoo(new foo);
$foo->testBar(new bar);
$foo->testBaz(new baz);
$foo->testFoo(new stdClass); // Recoverable fatal error

?>
--EXPECTF--
object(foo)#%d (0) {
}
object(bar)#%d (0) {
}
object(baz)#%d (0) {
}

Fatal error: Uncaught TypeError: foo::testFoo(): Argument #1 ($obj) must be of type foo, stdClass given, called in %s:%d
Stack trace:
#0 %s(%d): foo->testFoo(Object(stdClass))
#1 {main}
  thrown in %s on line %d
