--TEST--
017: Run-time name conflict and functions (php name)
--FILE--
<?php
namespace test\ns1;

function strlen($x) {
    return __FUNCTION__;
}

$x = "strlen";
echo $x("Hello"),"\n";
?>
--EXPECT--
5
