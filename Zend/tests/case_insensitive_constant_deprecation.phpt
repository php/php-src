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
}

namespace NS {
    var_dump(FOO); // Ok
    var_dump(foo); // Deprecated
}

namespace ns {
    var_dump(FOO); // Ok
    var_dump(foo); // Deprecated
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
int(24)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "NS\FOO" in %s on line 23
int(24)
int(24)

Deprecated: Case-insensitive constants are deprecated. The correct casing for this constant is "NS\FOO" in %s on line 28
int(24)

