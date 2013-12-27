--TEST--
Bug #66356 (Heap Overflow Vulnerability in imagecrop())
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip gd extension not available'); }
?>
--FILE--
<?php
$img = imagecreatetruecolor(10, 10);
$img = imagecrop($img, array("x" => "a", "y" => 0, "width" => 10, "height" => 10));
$arr = array("x" => "a", "y" => "12b", "width" => 10, "height" => 10);
$img = imagecrop($img, $arr);
print_r($arr);
?>
--EXPECTF--
Array
(
    [x] => a
    [y] => 12b
    [width] => 10
    [height] => 10
)
