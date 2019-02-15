--TEST--
Test natsort(): basic functionality
--FILE--
<?php
/*
* proto bool natsort ( array &$array )
* Function is implemented in ext/standard/array.c
*/
$array1 = $array2 = array("img12.png", "img10.png", "img2.png", "img1.png");
sort($array1);
echo "Standard sorting\n";
print_r($array1);
natsort($array2);
echo "\nNatural order sorting\n";
print_r($array2);
?>
--EXPECT--
Standard sorting
Array
(
    [0] => img1.png
    [1] => img10.png
    [2] => img12.png
    [3] => img2.png
)

Natural order sorting
Array
(
    [3] => img1.png
    [2] => img2.png
    [1] => img10.png
    [0] => img12.png
)
