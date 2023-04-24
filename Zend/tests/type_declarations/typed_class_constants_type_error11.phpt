--TEST--
Typed class constants (static type error)
--FILE--
<?php
enum E1 {
    const static C = E2::Foo;
}

enum E2 {
   case Foo;
}

try {
    var_dump(E1::C);
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Cannot assign E2 to class constant E1::C of type static
