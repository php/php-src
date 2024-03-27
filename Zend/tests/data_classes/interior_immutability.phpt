--TEST--
Basic data classes
--FILE--
<?php

data class Child {
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
--EXPECT--
Cannot modify readonly property Parent_::$readonlyChild
object(Parent_)#1 (2) {
  ["writableChild"]=>
  object(Child)#2 (1) {
    ["value"]=>
    int(2)
  }
  ["readonlyChild"]=>
  object(Child)#3 (1) {
    ["value"]=>
    int(1)
  }
}
