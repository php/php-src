--TEST--
ReflectionEnumBackedCase::getBackingValue() on a non-backed enum case
--FILE--
<?php

enum NotBacked {
    case A;
}

class FU extends ReflectionEnumBackedCase {
    public function __construct($class, $const) {
        ReflectionClassConstant::__construct($class, $const);
    }
}

try {
    (new FU('NotBacked', 'A'))->getBackingValue();
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Enum case NotBacked::A is not a backed case
