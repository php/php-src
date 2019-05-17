--TEST--
Bug #52719: array_walk_recursive crashes if third param of the function is by reference
--FILE--
<?php
$array = array("hello", array("world"));
$userdata = array();
array_walk_recursive(
    $array,
    function ($value, $key, &$userdata) { },
    $userdata
);
echo "Done";
?>
--EXPECT--
Done
