--TEST--
Foreach loop tests - error case: not an array.
--FILE--
<?php
echo "\nNot an array.\n";
$a = TRUE;
foreach ($a as $v) {
    var_dump($v);
}

$a = null;
foreach ($a as $v) {
    var_dump($v);
}

$a = 1;
foreach ($a as $v) {
    var_dump($v);
}

$a = 1.5;
foreach ($a as $v) {
    var_dump($v);
}

$a = "hello";
foreach ($a as $v) {
    var_dump($v);
}

echo "done.\n";
?>
--EXPECTF--
Not an array.

Warning: foreach() argument must be of type array|object, bool given in %s on line 4

Warning: foreach() argument must be of type array|object, null given in %s on line 9

Warning: foreach() argument must be of type array|object, int given in %s on line 14

Warning: foreach() argument must be of type array|object, float given in %s on line 19

Warning: foreach() argument must be of type array|object, string given in %s on line 24
done.
