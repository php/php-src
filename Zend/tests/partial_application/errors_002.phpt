--TEST--
Partial application errors: named parameter overwrites place holder
--FILE--
<?php
function foo($a) {

}

try {
    foo(?, a: 1);
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}
?>
--EXPECT--
Named parameter $a overwrites previous place holder

