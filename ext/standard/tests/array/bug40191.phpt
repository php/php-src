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
array_unique() expects parameter 1 to be array, object given
Done
