--TEST--
Test strcspn() behavior
--FILE--
<?php
$a = "22222222aaaa bbb1111 cccc";
$b = "1234";
var_dump($a);
var_dump($b);
var_dump(strcspn($a,$b));
var_dump(strcspn($a,$b,9));
var_dump(strcspn($a,$b,9,6));
try {
    var_dump(strcspn('a', 'B', 1, 2147483647));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
string(25) "22222222aaaa bbb1111 cccc"
string(4) "1234"
int(0)
int(7)
int(6)
strcspn(): Argument #4 ($length) must be contained in argument #1 ($str)
