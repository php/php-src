--TEST--
Basic structs
--FILE--
<?php

struct Child {
    public function __construct(
        public $value,
    ) {}
}

class Parent_ {
    public function __construct(
        public Child $writableChild,
        public readonly Child $readonlyChild,
    ) {}
}

$parent = new Parent_(new Child(1), new Child(1));
$parent->writableChild->value = 2;
try {
    $parent->readonlyChild->value = 2;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump($parent);

?>
--EXPECTF--
Cannot indirectly modify readonly property Parent_::$readonlyChild
object(Parent_)#1 (2) {
  ["writableChild"]=>
  object(Child)#%d (1) {
    ["value"]=>
    int(2)
  }
  ["readonlyChild"]=>
  object(Child)#%d (1) {
    ["value"]=>
    int(1)
  }
}
