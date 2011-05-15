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
==DONE==
<?php exit(0); ?>
--EXPECTF--
Fatal error: Uncaught exception 'LogicException' with message 'In the constructor of foo, parent::__construct() must be called and its exceptions cannot be cleared' in %s:%d
Stack trace:
#0 %s(%d): foo->internal_construction_wrapper('This is bar')
#1 {main}
  thrown in %s on line %d
