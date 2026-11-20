--TEST--
GH-25340 (ReflectionEnumBackedCase can be used on a non-backed enum via a constructor trick)
--CREDITS--
ndossche
--FILE--
<?php

class Plain { const X = 1; }

enum NotBacked { case A; }

enum Backed: int {
    case A = 1;
    const C = 2;
}

class U extends ReflectionEnumUnitCase {
    public function __construct($class, $const) {
        ReflectionClassConstant::__construct($class, $const);
    }
}

class B extends ReflectionEnumBackedCase {
    public function __construct($class, $const) {
        ReflectionClassConstant::__construct($class, $const);
    }
}

foreach ([[U::class, 'Plain', 'X'], [B::class, 'NotBacked', 'A'], [B::class, 'Backed', 'C']] as [$reflector, $class, $const]) {
    try {
        new $reflector($class, $const);
    } catch (ReflectionException $e) {
        echo $e->getMessage(), "\n";
    }
}

$r = new B('Backed', 'A');
var_dump($r->getEnum()->name, $r->getValue(), $r->getBackingValue());

try {
    $r->__construct('NotBacked', 'A');
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($r->getBackingValue());

?>
--EXPECT--
Constant Plain::X is not a case
Enum case NotBacked::A is not a backed case
Constant Backed::C is not a case
string(6) "Backed"
enum(Backed::A)
int(1)
Enum case NotBacked::A is not a backed case
int(1)
