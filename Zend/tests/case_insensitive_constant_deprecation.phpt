--TEST--
Case-insensitive constants are deprecated
--FILE--
<?php

namespace {
    define('FOO', 42, true); // Deprecated
    define('NS\FOO', 24, true); // Deprecated

    var_dump(FOO); // Ok
    var_dump(foo); // Deprecated

    var_dump(NS\FOO); // Ok
    var_dump(ns\FOO); // Ok
    var_dump(ns\foo); // Deprecated

    var_dump(defined('FOO')); // Ok
    var_dump(defined('foo')); // Ok
    var_dump(defined('NS\FOO')); // Ok
    var_dump(defined('ns\FOO')); // Ok
    var_dump(defined('ns\foo')); // Ok

    var_dump(constant('FOO')); // Ok
    var_dump(constant('foo')); // Deprecated
    var_dump(constant('NS\FOO')); // Ok
    var_dump(constant('ns\FOO')); // Ok
    var_dump(constant('ns\foo')); // Deprecated
}

namespace NS {
    var_dump(FOO); // Ok
    var_dump(foo); // Deprecated
}

namespace ns {
    var_dump(FOO); // Ok
    var_dump(foo); // Deprecated
}

namespace Other {
    var_dump(FOO); // Ok
    var_dump(foo); // Deprecated

    var_dump(defined('FOO')); // Ok
    var_dump(defined('foo')); // Ok
    var_dump(defined('NS\FOO')); // Ok
    var_dump(defined('ns\FOO')); // Ok
    var_dump(defined('ns\foo')); // Ok

    var_dump(constant('FOO')); // Ok
    var_dump(constant('foo')); // Deprecated
    var_dump(constant('NS\FOO')); // Ok
    var_dump(constant('ns\FOO')); // Ok
    var_dump(constant('ns\foo')); // Deprecated

    const C1 = FOO; // Ok
    var_dump(C1);
    const C2 = foo; // Deprecated
    var_dump(C2);
    const C3 = 1 + FOO; // Ok
    var_dump(C3);
    const C4 = 1 + foo; // Deprecated
    var_dump(C4);
}

?>
--EXPECTF--
Deprecated: define(): Declaration of case-insensitive constants is deprecated in %s on line 4

Deprecated: define(): Declaration of case-insensitive constants is deprecated in %s on line 5
int(42)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "FOO" in %s on line 8
int(42)
int(24)
int(24)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "NS\FOO" in %s on line 12
int(24)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(42)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "FOO" in %s on line 21
int(42)
int(24)
int(24)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "NS\FOO" in %s on line 24
int(24)
int(24)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "NS\FOO" in %s on line 29
int(24)
int(24)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "NS\FOO" in %s on line 34
int(24)
int(42)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "FOO" in %s on line 39
int(42)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(42)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "FOO" in %s on line 48
int(42)
int(24)
int(24)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "NS\FOO" in %s on line 51
int(24)
int(42)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "FOO" in %s on line 55
int(42)
int(43)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "FOO" in %s on line 59
int(43)
