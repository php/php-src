--TEST--
Anonymous class with __invoke() auto-implements Invokable
--FILE--
<?php

$obj = new class {
    public function __invoke(): string {
        return "anonymous";
    }
};

var_dump($obj instanceof Invokable);
var_dump($obj());

/* Anonymous class with explicit implements */
$obj2 = new class implements Invokable {
    public function __invoke(): int {
        return 42;
    }
};

var_dump($obj2 instanceof Invokable);
var_dump($obj2());

?>
--EXPECT--
bool(true)
string(9) "anonymous"
bool(true)
int(42)
