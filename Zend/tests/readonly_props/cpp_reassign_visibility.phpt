--TEST--
Promoted readonly property reassignment in constructor - visibility rules apply
--FILE--
<?php

// Case 1: private(set) - child cannot reassign
class Parent1 {
    public function __construct(
        private(set) public readonly string $prop = 'parent default',
    ) {
        // Parent doesn't use reassignment
    }
}

class Child1 extends Parent1 {
    public function __construct() {
        parent::__construct();
        // Child cannot reassign - private(set) restricts to declaring class only
        try {
            $this->prop = 'child override';
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$child1 = new Child1();
var_dump($child1->prop);

// Case 2: protected(set) - child CAN reassign
class Parent2 {
    public function __construct(
        protected(set) public readonly string $prop = 'parent default',
    ) {
        // Parent doesn't use reassignment
    }
}

class Child2 extends Parent2 {
    public function __construct() {
        parent::__construct();
        // Child CAN reassign - protected(set) allows child classes
        $this->prop = 'child override';
    }
}

$child2 = new Child2();
var_dump($child2->prop);

// Case 3: public (default) - child CAN reassign
class Parent3 {
    public function __construct(
        public readonly string $prop = 'parent default',
    ) {
        // Parent doesn't use reassignment
    }
}

class Child3 extends Parent3 {
    public function __construct() {
        parent::__construct();
        // Child CAN reassign - public allows anyone
        $this->prop = 'child override';
    }
}

$child3 = new Child3();
var_dump($child3->prop);

// Case 4: protected(set) with parent using reassignment - child cannot (one reassignment rule)
class Parent4 {
    public function __construct(
        protected(set) public readonly string $prop = 'parent default',
    ) {
        $this->prop = 'parent set';  // Uses the one reassignment
    }
}

class Child4 extends Parent4 {
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

$child4 = new Child4();
var_dump($child4->prop);

?>
--EXPECT--
Cannot modify private(set) property Parent1::$prop from scope Child1
string(14) "parent default"
string(14) "child override"
string(14) "child override"
Cannot modify readonly property Parent4::$prop
string(10) "parent set"
