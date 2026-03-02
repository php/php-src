--TEST--
Immediate access on new anonymous class
--FILE--
<?php

echo new class {
    const C = 'constant' . PHP_EOL;
}::C;

echo new class {
    const C = 'constant' . PHP_EOL;
}::{'C'};

echo new class {
    public $property = 'property' . PHP_EOL;
}->property;

echo new class {
    public static $property = 'static property' . PHP_EOL;
}::$property;

new class {
    public function method() { echo 'method' . PHP_EOL; }
}->method();

new class {
    public static function method() { echo 'static method' . PHP_EOL; }
}::method();

new class {
    public function __invoke() { echo '__invoke' . PHP_EOL; }
}();

new class () implements ArrayAccess {
    public function offsetExists(mixed $offset): bool { return true; }

    public function offsetGet(mixed $offset): mixed { echo 'offsetGet' . PHP_EOL; return null; }

    public function offsetSet(mixed $offset, mixed $value): void {}

    public function offsetUnset(mixed $offset): void {}
}['key'];

isset(new class () implements ArrayAccess {
    public function offsetExists(mixed $offset): bool { echo 'offsetExists' . PHP_EOL; return true; }

    public function offsetGet(mixed $offset): mixed { return null; }

    public function offsetSet(mixed $offset, mixed $value): void {}

    public function offsetUnset(mixed $offset): void {}
}['key']);

?>
--EXPECT--
constant
constant
property
static property
method
static method
__invoke
offsetGet
offsetExists
