--TEST--
Testing null byte injection in imagegd
--SKIPIF--
<?php
        if(!extension_loaded('gd')){ die('skip gd extension not available'); }
?>
--FILE--
<?php
$image = imagecreate(1,1);// 1px image
var_dump(imagegd($image, "./foo\0bar"));
?>
--EXPECTF--
Warning: imagegd() expects parameter 2 to be a valid path, string given in %s on line %d
NULL
