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
try {
    echo $foo->bar();
} catch (Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
The foo instance wasn't initialized properly
