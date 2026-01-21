--TEST--
Promoted readonly property reassignment in constructor - child class cannot reassign parent's property
--FILE--
<?php

class Parent_ {
    public function __construct(
        public readonly string $prop = 'parent default',
    ) {
        // Parent can reassign its own promoted property
        $this->prop = 'parent set';
    }
}

class Child extends Parent_ {
    public function __construct() {
        parent::__construct();
        // Child cannot reassign parent's promoted property
        try {
            $this->prop = 'child override';
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$parent = new Parent_();
var_dump($parent->prop);

$child = new Child();
var_dump($child->prop);

// Even when child has its own promoted property
class Parent2 {
    public function __construct(
        public readonly string $parentProp = 'parent default',
    ) {
        $this->parentProp = 'parent set';
    }
}

class Child2 extends Parent2 {
    public function __construct(
        public readonly string $childProp = 'child default',
    ) {
        parent::__construct();
        // Child can reassign its own promoted property
        $this->childProp = 'child set';
        // But cannot reassign parent's promoted property
        try {
            $this->parentProp = 'child override';
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$child2 = new Child2();
var_dump($child2->parentProp, $child2->childProp);

?>
--EXPECT--
string(10) "parent set"
Cannot modify readonly property Parent_::$prop
string(10) "parent set"
Cannot modify readonly property Parent2::$parentProp
string(10) "parent set"
string(9) "child set"
