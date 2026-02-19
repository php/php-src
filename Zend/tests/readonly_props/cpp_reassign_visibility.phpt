--TEST--
Promoted readonly property reassignment in constructor - visibility rules apply
--FILE--
<?php

// Case 1: private(set) - child's init() cannot modify even within CPP window
class Parent1 {
    public function __construct(
        private(set) public readonly string $prop = 'parent default',
    ) {
        $this->init();
    }

    protected function init(): void {
    }
}

class Child1 extends Parent1 {
    protected function init(): void {
        try {
            $this->prop = 'child override';
        } catch (Throwable $e) {
            echo get_class($e), ": ", $e->getMessage(), "\n";
        }
    }
}

$child1 = new Child1();
var_dump($child1->prop);

// Case 2: protected(set) - child's init() CAN modify within CPP window
class Parent2 {
    public function __construct(
        protected(set) public readonly string $prop = 'parent default',
    ) {
        $this->init();
    }

    protected function init(): void {
    }
}

class Child2 extends Parent2 {
    protected function init(): void {
        $this->prop = 'child set';
    }
}

$child2 = new Child2();
var_dump($child2->prop);

// Case 3: public(set) - child's init() CAN modify within CPP window
class Parent3 {
    public function __construct(
        public public(set) readonly string $prop = 'parent default',
    ) {
        $this->init();
    }

    protected function init(): void {
    }
}

class Child3 extends Parent3 {
    protected function init(): void {
        $this->prop = 'child set';
    }
}

$child3 = new Child3();
var_dump($child3->prop);

// Case 4: protected(set) with parent using reassignment - child cannot (window closed)
class Parent4 {
    public function __construct(
        protected(set) public readonly string $prop = 'parent default',
    ) {
        $this->prop = 'parent set';  // Uses the one reassignment
        $this->init();
    }

    protected function init(): void {
    }
}

class Child4 extends Parent4 {
    protected function init(): void {
        try {
            $this->prop = 'child override';
        } catch (Throwable $e) {
            echo get_class($e), ": ", $e->getMessage(), "\n";
        }
    }
}

$child4 = new Child4();
var_dump($child4->prop);

?>
--EXPECT--
Error: Cannot modify private(set) property Parent1::$prop from scope Child1
string(14) "parent default"
string(9) "child set"
string(9) "child set"
Error: Cannot modify readonly property Parent4::$prop
string(10) "parent set"
