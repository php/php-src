--TEST--
Readonly property with default value
--FILE--
<?php

enum E {
    case Case;
}

class Test {
    public readonly int $prop = 1;
    public readonly string $className = self::class;
    public readonly ?string $nullable = null;
    public readonly array $array = [1, "two" => 2];
    public readonly E $enum = E::Case;
    public readonly string $enumString = E::Case->name;
}

$test = new Test;
var_dump($test->prop);
var_dump($test->className);
var_dump($test->nullable);
var_dump($test->array);
var_dump($test->enum);
var_dump($test->enumString);
try {
    $test->prop = 2;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
int(1)
string(4) "Test"
NULL
array(2) {
  [0]=>
  int(1)
  ["two"]=>
  int(2)
}
enum(E::Case)
string(4) "Case"
Error: Cannot modify readonly property Test::$prop
