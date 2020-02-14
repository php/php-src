--TEST--
Can call internal functions from parameter default
--FILE--
<?php
namespace NS;
use function max;
function test_default($x = max(1, 3, 2)) {
    echo "x is $x\n";
}
test_default();
test_default(2);
test_default();
?>
--EXPECT--
x is 3
x is 2
x is 3