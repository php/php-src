--TEST--
Bug #61173: Unable to detect error from finfo constructor
--SKIPIF--
<?php
if (!class_exists('finfo'))
    die('skip no fileinfo extension');
--FILE--
<?php

try {
    $finfo = new finfo(1, '', false);
    var_dump($finfo);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
finfo::__construct() expects at most 2 arguments, 3 given
