--TEST--
GH-25340 (ReflectionEnumBackedCase can be used on a non-backed enum via a constructor trick)
--CREDITS--
ndossche
--FILE--
<?php

enum NotBacked { case A; }

enum Backed: int {
    case A = 1;
    const C = 2;
}

class FU extends ReflectionEnumBackedCase {
    public function __construct($class, $const) {
        ReflectionClassConstant::__construct($class, $const);
    }
}

foreach ([['NotBacked', 'A'], ['Backed', 'C'], ['Backed', 'A']] as [$class, $const]) {
    try {
        var_dump((new FU($class, $const))->getBackingValue());
    } catch (ReflectionException $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
Enum case NotBacked::A is not a backed case
Constant Backed::C is not a case
int(1)
