--TEST--
Backed enums: user-defined values() overrides native
--FILE--
<?php

enum E: int {
    case A = 1;
    case B = 2;

    public static function values(): array {
        return ['custom'];
    }
}

var_dump(E::values());

?>
--EXPECT--
array(1) {
  [0]=>
  string(6) "custom"
}

