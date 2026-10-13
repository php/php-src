--TEST--
GH-24147 (ReflectionProperty class and name properties can be changed)
--FILE--
<?php

class B {
    public string $x;
}

function byVal($x) {}
function byRef(&$x) { $x .= 'Z'; }

$prop = new ReflectionProperty('B', 'x');

try {
    $prop->class .= 'Z';
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $prop->name++;
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $ref = &$prop->name;
    $ref = 'y';
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

foreach (['byVal', 'byRef'] as $fn) {
    try {
        $fn($prop->class);
    } catch (ReflectionException $e) {
        echo $e->getMessage(), "\n";
    }
}

var_dump($prop->class, $prop->name);

?>
--EXPECT--
Cannot set read-only property ReflectionProperty::$class
Cannot set read-only property ReflectionProperty::$name
Cannot set read-only property ReflectionProperty::$name
Cannot set read-only property ReflectionProperty::$class
string(1) "B"
string(1) "x"
