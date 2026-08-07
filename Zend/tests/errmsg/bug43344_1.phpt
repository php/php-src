--TEST--
Bug #43344.1 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
use Error;

function f1($a=bar) {
    return $a;
}
function f2($a=array(bar)) {
    return $a[0];
}
function f3($a=array(bar=>0)) {
    reset($a);
    return key($a);
}

try {
    echo bar."\n";
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    echo f1()."\n";
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    echo f2()."\n";
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    echo f3()."\n";
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Undefined constant "Foo\bar"
Error: Undefined constant "Foo\bar"
Error: Undefined constant "Foo\bar"
Error: Undefined constant "Foo\bar"
