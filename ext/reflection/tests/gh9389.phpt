--TEST--
GH-9389: Typed property /w magic __get cannot be uninitialized
--FILE--
<?php

class Cl {
    public string $name;
    
    public function __get(string $n): void
    {
        var_dump('__get: ' . $n);
    }
}

$obj = new Cl();

// read on uninitialized typed property - default
try {
    $obj->name;
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

// read on unset typed property
unset($obj->name);
try {
    $obj->name;
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

// read of uninitialized typed property - after unset
(new ReflectionProperty($obj, 'name'))->uninitializeValue($obj);
try {
    $obj->name;
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

// read on set typed property
$obj->name = 'foo';
var_dump($obj->name);

// read of uninitialized typed property - after set
(new ReflectionProperty($obj, 'name'))->uninitializeValue($obj);
try {
    $obj->name;
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Typed property Cl::$name must not be accessed before initialization
string(11) "__get: name"
Cannot assign null to property Cl::$name of type string
Typed property Cl::$name must not be accessed before initialization
string(3) "foo"
Typed property Cl::$name must not be accessed before initialization
