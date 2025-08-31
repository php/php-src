--TEST--
076: Unknown constants in namespace
--FILE--
<?php
namespace foo;
use Error;

try {
    $a = array(unknown => unknown);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    echo unknown;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    echo \unknown;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Undefined constant "foo\unknown"
Undefined constant "foo\unknown"
Undefined constant "unknown"
