--TEST--
GH-13433 (Segmentation Fault in zend_class_init_statics when using opcache.preload)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0
opcache.preload={PWD}/preload.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
$instance = new TheClass;
var_dump($instance);
var_dump($instance->test());
?>
OK
--EXPECT--
object(TheClass)#1 (0) {
}
array(1) {
  [0]=>
  string(9) "non-empty"
}
OK
