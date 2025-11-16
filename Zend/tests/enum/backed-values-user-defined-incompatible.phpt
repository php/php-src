--TEST--
Backed enums: user-defined values() with different return type is allowed
--FILE--
<?php

enum E: string {
    case A = 'a';

    // Interface has no return type, so any return type is allowed
    public static function values(): string {
        return 'custom_values';
    }
}

var_dump(E::values());

?>
--EXPECT--
string(13) "custom_values"

