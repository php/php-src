--TEST--
Promoted readonly property reassignment - grandchild non-CPP redeclare preserves child CPP window
--FILE--
<?php

class P {
    public function __construct(
        public readonly string $x = 'P',
    ) {}
}

class C extends P {
    public function __construct(
        public readonly string $x = 'C',
    ) {
        try {
            parent::__construct();
        } catch (Throwable $e) {
            echo get_class($e), ": ", $e->getMessage(), "\n";
        }
        $this->x = 'C2';
    }
}

class D extends C {
    public readonly string $x;

    public function __construct() {
        parent::__construct();
        var_dump($this->x);
    }
}

new D();

?>
--EXPECT--
Error: Cannot modify readonly property D::$x
string(2) "C2"
