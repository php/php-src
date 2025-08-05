--TEST--
PFA errors: named parameter that resolve to the position of a placeholder is an error
--FILE--
<?php
function foo($a) {
}

try {
    foo(?, a: 1);
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}
?>
--EXPECT--
Error: Named parameter $a overwrites previous placeholder
