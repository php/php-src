--TEST--
Bug #53362 (Segmentation fault when extending SplFixedArray)
--FILE--
<?php

class obj extends SplFixedArray{
    public function offsetSet($offset, $value) {
        var_dump($offset);
    }
}

$obj = new obj;

$obj[]=2;
$obj[]=2;
$obj[]=2;

?>
--EXPECT--
NULL
NULL
NULL
