--TEST--
basic array_diff_assoc test
--FILE--	
<?php
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red", "");
$array2 = array("a" => "green", "yellow", "red", TRUE);
$array3 = array("red", "a"=>"brown", "");
$result[] = array_diff_assoc($array1, $array2);
$result[] = array_diff_assoc($array1, $array3);
$result[] = array_diff_assoc($array2, $array3);
$result[] = array_diff_assoc($array1, $array2, $array3);
print_r($result)
?>
--EXPECT--
Array
(
    [0] => Array
        (
            [b] => brown
            [c] => blue
            [0] => red
            [1] => 
        )

    [1] => Array
        (
            [a] => green
            [b] => brown
            [c] => blue
        )

    [2] => Array
        (
            [a] => green
            [0] => yellow
            [1] => red
            [2] => 1
        )

    [3] => Array
        (
            [b] => brown
            [c] => blue
        )

)



