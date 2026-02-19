--TEST--
Promoted readonly property reassignment in constructor - child redefines parent property
--FILE--
<?php

// Case 1: Parent uses CPP, child redefines as non-promoted, child tries to reassign.
// The property is owned by C1; parent CPP sets its initial value, but child's
// non-promoted redefinition means no reassignment window exists for the child.
class P1 {
    public function __construct(
        public readonly string $x = 'P',
    ) {}
}

class C1 extends P1 {
    public readonly string $x;

    public function __construct() {
        parent::__construct();
        try {
            $this->x = 'C';
        } catch (Throwable $e) {
            echo get_class($e), ": ", $e->getMessage(), "\n";
        }
    }
}

$c1 = new C1();
var_dump($c1->x);

// Case 2: Parent uses CPP and reassigns; child redefines as non-promoted.
// The child does not use CPP, so it does not claim CPP ownership of the property.
// P2's CPP "owns" the reassignment window: P2's body write succeeds.
class P2 {
    public function __construct(
        public readonly string $x = 'P1',
    ) {
        $this->x = 'P2';
    }
}

class C2 extends P2 {
    public readonly string $x;

    public function __construct() {
        parent::__construct();
    }
}

$c2 = new C2();
var_dump($c2->x);

// Case 3: Parent uses CPP, child uses CPP redefinition.
// Child's CPP opens the reassignment window for C3::$x. When parent::__construct()
// runs, P3's CPP tries to initialize C3::$x again, which must fail since C3
// owns the property and has already initialized it.
class P3 {
    public function __construct(
        public readonly string $x = 'P',
    ) {}
}

class C3 extends P3 {
    public function __construct(
        public readonly string $x = 'C1',
    ) {
        try {
            parent::__construct();
        } catch (Throwable $e) {
            echo get_class($e), ": ", $e->getMessage(), "\n";
        }
    }
}

$c3 = new C3();
var_dump($c3->x);

?>
--EXPECT--
Error: Cannot modify readonly property C1::$x
string(1) "P"
string(2) "P2"
Error: Cannot modify readonly property C3::$x
string(2) "C1"
