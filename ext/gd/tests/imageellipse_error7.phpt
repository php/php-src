--TEST--
Testing wrong param passing imageellipse() of GD library
--CREDITS--
Ivan Rosolen <contato [at] ivanrosolen [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

// Create a resource
$image = tmpfile();

// try to draw a white ellipse
try {
    imageellipse($image, 200, 150, 300, 200, 16777215);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imageellipse(): supplied resource is not a valid Image resource
