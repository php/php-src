--TEST--
(Bug #74031) ReflectionFunction for imagepng returns wrong number of parameters
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!(imagetypes() & IMG_PNG)) {
    die("skip No PNG support");
}
?>
--FILE--
<?php

$ref = new ReflectionFunction('imagepng');
var_dump(count($ref->getParameters()));
?>
--EXPECT--
int(4)
