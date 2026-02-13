--TEST--
Promoted readonly property reassignment in constructor - child cannot reassign parent's property
--FILE--
<?php

// Case 1: Parent does NOT use reassignment, child still cannot reassign
class Parent1 {
    public function __construct(
        public readonly string $prop = 'parent default',
    ) {
        // Parent does NOT reassign here - leaves opportunity for child
    }
}

class Child1 extends Parent1 {
    public function __construct() {
        parent::__construct();
        // Child cannot reassign parent-owned promoted property
        try {
            $this->prop = 'child override';
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$parent = new Parent1();
var_dump($parent->prop);

$child = new Child1();
var_dump($child->prop);

// Case 2: Parent USES reassignment, child cannot
class Parent2 {
    public function __construct(
        public readonly string $prop = 'parent default',
    ) {
        $this->prop = 'parent set';  // Uses the one reassignment
    }
}

class Child2 extends Parent2 {
    public function __construct() {
        parent::__construct();
        // Child cannot reassign - parent already used the one reassignment
        try {
            $this->prop = 'child override';
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$child2 = new Child2();
var_dump($child2->prop);

// Case 3: Child with its own promoted property
class Parent3 {
    public function __construct(
        public readonly string $parentProp = 'parent default',
    ) {
        // Parent doesn't reassign
    }
}

class Child3 extends Parent3 {
    public function __construct(
        public readonly string $childProp = 'child default',
    ) {
        parent::__construct();
        // Child cannot reassign parent's property, but can reassign its own
        try {
            $this->parentProp = 'child set parent';
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        $this->childProp = 'child set own';
    }
}

$child3 = new Child3();
var_dump($child3->parentProp, $child3->childProp);

?>
--EXPECT--
string(14) "parent default"
Cannot modify readonly property Parent1::$prop
string(14) "parent default"
Cannot modify readonly property Parent2::$prop
string(10) "parent set"
Cannot modify readonly property Parent3::$parentProp
string(14) "parent default"
string(13) "child set own"
