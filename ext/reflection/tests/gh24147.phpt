--TEST--
GH-24147: Reflection $name and $class properties cannot be modified
--FILE--
<?php

class B {
    public string $x;
}

function byRef(&$x) { $x = 'Z'; }

$tests = [
    'compound assignment' => function ($r) { $r->class .= 'Z'; },
    'reference' => function ($r) { $ref = &$r->name; $ref = 'Z'; },
    'string offset' => function ($r) { $r->name[0] = 'Z'; },
    'by-ref argument' => function ($r) { byRef($r->class); },
    'dynamic by-ref argument' => function ($r) { $fn = 'byRef'; $fn($r->class); },
    'foreach by reference' => function ($r) { foreach ($r as &$v) { $v = 'Z'; } },
    'unset' => function ($r) { unset($r->name); },
    'setValue' => function ($r) { (new ReflectionProperty($r, 'name'))->setValue($r, 'Z'); },
    'setRawValue' => function ($r) { (new ReflectionProperty($r, 'name'))->setRawValue($r, 'Z'); },
];

foreach ($tests as $label => $test) {
    $r = new ReflectionProperty('B', 'x');
    try {
        $test($r);
        echo "$label: no error\n";
    } catch (Throwable $e) {
        echo "$label: ", $e::class, ": ", $e->getMessage(), "\n";
    }
    if (!isset($r->name, $r->class) || $r->name !== $r->getName() || $r->class !== $r->getDeclaringClass()->name) {
        echo "$label: properties diverged from getters\n";
    }
}

class WriteBeforeConstruct extends ReflectionClass {
    public function __construct() {
        $this->name = 'Z';
    }
}

class UnsetAfterConstruct extends ReflectionClass {
    public function __construct() {
        parent::__construct('B');
        unset($this->name);
    }
}

foreach (['WriteBeforeConstruct', 'UnsetAfterConstruct'] as $class) {
    try {
        new $class();
        echo "$class: no error\n";
    } catch (Throwable $e) {
        echo "$class: ", $e::class, ": ", $e->getMessage(), "\n";
    }
}

// Unsetting the uninitialized property in a subclass to route reads through __get() stays possible
class UnsetBeforeConstruct extends ReflectionClass {
    public function __construct() {
        unset($this->name);
    }

    public function __get($name) {
        return "magic $name";
    }
}

var_dump((new UnsetBeforeConstruct())->name);

foreach (['setValue', 'setRawValue'] as $method) {
    $r = (new ReflectionClass('ReflectionClass'))->newInstanceWithoutConstructor();
    try {
        (new ReflectionProperty($r, 'name'))->$method($r, 'Z');
        echo "uninitialized $method: no error\n";
    } catch (Throwable $e) {
        echo "uninitialized $method: ", $e::class, ": ", $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
compound assignment: ReflectionException: Cannot set read-only property ReflectionProperty::$class
reference: Error: Cannot indirectly modify readonly property ReflectionProperty::$name
string offset: Error: Cannot indirectly modify readonly property ReflectionProperty::$name
by-ref argument: Error: Cannot indirectly modify readonly property ReflectionProperty::$class
dynamic by-ref argument: Error: Cannot indirectly modify readonly property ReflectionProperty::$class
foreach by reference: Error: Cannot acquire reference to readonly property ReflectionProperty::$name
unset: Error: Cannot unset readonly property ReflectionProperty::$name
setValue: ReflectionException: Cannot set read-only property ReflectionProperty::$name
setRawValue: ReflectionException: Cannot set read-only property ReflectionProperty::$name
WriteBeforeConstruct: ReflectionException: Cannot set read-only property WriteBeforeConstruct::$name
UnsetAfterConstruct: Error: Cannot unset readonly property ReflectionClass::$name
string(10) "magic name"
uninitialized setValue: ReflectionException: Cannot set read-only property ReflectionClass::$name
uninitialized setRawValue: ReflectionException: Cannot set read-only property ReflectionClass::$name
