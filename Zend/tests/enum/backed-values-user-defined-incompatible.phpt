--TEST--
Backed enums: user-defined values() incompatible with interface signature
--FILE--
<?php

enum E: string {
    case A = 'a';

    // Intentional incompatibility: interface requires array return type
    public static function values(): string {
        return 'values';
    }
}

?>
--EXPECTF--
Fatal error: Declaration of E::values(): string must be compatible with BackedEnum::values(): array in %s on line %d

