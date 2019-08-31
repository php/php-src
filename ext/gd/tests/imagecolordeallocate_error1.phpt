--TEST--
Testing imagecolordeallocate() of GD library with invalid resource type
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$image = imagecreatetruecolor(180, 30);
$white = imagecolorallocate($image, 255, 255, 255);

$resource = tmpfile();

try {
    $result = imagecolordeallocate($resource, $white);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
imagecolordeallocate(): supplied resource is not a valid Image resource
