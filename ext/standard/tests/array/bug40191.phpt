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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
TypeError: array_unique(): Argument #1 ($array) must be of type array, ArrayObject given
Done
