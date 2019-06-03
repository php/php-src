--TEST--
Testing error on non-image resource parameter 1 of imagestring() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

try {
    $result = imagestring(tmpfile(), 1, 5, 5, 'String', 1);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
imagestring(): supplied resource is not a valid Image resource
