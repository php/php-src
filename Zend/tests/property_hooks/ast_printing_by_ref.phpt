--TEST--
By-reference hook AST printing
--FILE--
<?php

try {
    assert(false && new class {
        public $prop1 { &get => $this->prop1; }
        public $prop2 {
            final &get {
                return $this->prop2;
            }
        }
        public private(set) int $prop3 { &get => $this->prop3; }
    });
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
assert(false && new class {
    public $prop1 {
        &get => $this->prop1;
    }
    public $prop2 {
        final &get {
            return $this->prop2;
        }
    }
    public private(set) int $prop3 {
        &get => $this->prop3;
    }
})
