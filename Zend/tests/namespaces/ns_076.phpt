--TEST--
076: Unknown constants in namespace
--FILE--
<?php
namespace foo;
use Error;

try {
    $a = array(unknown => unknown);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    echo unknown;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    echo \unknown;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Undefined constant "foo\unknown"
Error: Undefined constant "foo\unknown"
Error: Undefined constant "unknown"
