--TEST--
Promoted readonly properties cannot be reassigned when __construct() is called directly
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly string $value = 'default',
    ) {
        $this->value = strtoupper($this->value);
    }
}

$obj = new Foo('hello');
var_dump($obj->value);

// Direct call fails: CPP assignment cannot reinitialize an already-set property
try {
    $obj->__construct('world');
} catch (Throwable $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}
var_dump($obj->value);

?>
--EXPECT--
string(5) "HELLO"
Error: Cannot modify readonly property Foo::$value
string(5) "HELLO"
