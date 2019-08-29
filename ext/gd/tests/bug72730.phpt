--TEST--
Bug #72730: imagegammacorrect allows arbitrary write access
--SKIPIF--
<?php
if (!function_exists("imagecreatetruecolor")) die("skip");
?>
--FILE--
<?php

require __DIR__ . '/test_helpers.inc';

$img =  imagecreatetruecolor(1, 1);

trycatch_dump(
    fn() => imagegammacorrect($img, -1, 1337)
);

?>
--EXPECT--
!! [Error] Gamma values must be positive
