--TEST--
Bug #41828 (Segfault if extended constructor of RecursiveIterator doesn't call its parent)
--FILE--
<?php
class foo extends RecursiveIteratorIterator {

    public function __construct($str) {
    }

    public function bar() {
    }
}

$foo = new foo("This is bar");
echo $foo->bar();

?>
--EXPECTF--
Fatal error: Uncaught Error: The foo instance wasn't initialized properly in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
