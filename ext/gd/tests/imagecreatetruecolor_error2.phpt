--TEST--
Testing imagecreatetruecolor(): error on out of bound parameters
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php

try {
    var_dump(imagecreatetruecolor(-1, 30));
}
catch (\Error $ex) {
    echo '[' . get_class($ex) . '] ' . $ex->getMessage() . "\n";
}

try {
    var_dump(imagecreatetruecolor(30, -1));
}
catch (\Error $ex) {
    echo '[' . get_class($ex) . '] ' . $ex->getMessage() . "\n";
}

?>
--EXPECT--
[Error] Invalid width (x_size)
[Error] Invalid height (y_size)
