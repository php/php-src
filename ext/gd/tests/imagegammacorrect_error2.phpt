--TEST--
Testing error with non-Image resource paramenter of imagegammacorrect() of GD library,
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$image = tmpfile();
try {
    $gamma = imagegammacorrect($image, 1, 5);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
imagegammacorrect(): supplied resource is not a valid Image resource
