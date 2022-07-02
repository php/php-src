--TEST--
Bug #40191 (use of array_unique() with objects triggers segfault)
--FILE--
<?php

$arrObj = new ArrayObject();
$arrObj->append('foo');
$arrObj->append('bar');
$arrObj->append('foo');

try {
    $arr = array_unique($arrObj);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
array_unique(): Argument #1 ($array) must be of type array, ArrayObject given
Done
