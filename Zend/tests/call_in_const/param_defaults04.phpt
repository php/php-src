--TEST--
Functions must be unambiguously in the whitelist
--FILE--
<?php
namespace NS;
function test_default($x = max(1, 3, 2)) {
    echo "x is $x\n";
}
test_default();
test_default(2);
test_default();
?>
--EXPECTF--
Fatal error: Constant expression uses function NS\max() which is not in get_const_expr_functions() in %s on line 3
