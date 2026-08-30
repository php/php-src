--TEST--
Test array_diff when non-array is passed
--FILE--
<?php
//-=-=-=-=-
$a = array();
$b = 3;
$c = array(5);
try {
    array_diff($a, $b, $c);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
//-=-=-=-=-=-
echo "OK!";
?>
--EXPECT--
TypeError: array_diff(): Argument #2 must be of type array, int given
OK!
