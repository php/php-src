--TEST--
GH-16358 (Segmentation fault (access null pointer) in Zend/zend_operators.c:2495)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1214
opcache.jit_buffer_size=64M
--FILE--
<?php
class test_class {
static protected function test_func4() {
}
}
if (is_callable(array('test_class','test_func4'))) {
test_class::test_func4();
}
?>
OK
--EXPECT--
OK
