--TEST--
Bug #34786 (2 @ results in change to error_reporting() to random value)
--FILE--
<?php
function foo($a,$b,$c) {
echo "foo: ".error_reporting()."\n";
}

function bar() {
echo "bar: ".error_reporting()."\n";
}

error_reporting(E_WARNING);
echo "before: ".error_reporting()."\n";
@foo(1,@bar(),3);
echo "after: ".error_reporting()."\n";
?>
--EXPECT--
before: 2
bar: 0
foo: 0
after: 2
