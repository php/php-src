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
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
//-=-=-=-=-=-
echo "OK!";
?>
--EXPECT--
array_diff(): Argument #2 must be of type array, int given
OK!
