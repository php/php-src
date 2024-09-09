--TEST--
GH-15658 (Segmentation fault in Zend/zend_vm_execute.h)
--EXTENSIONS--
opcache
--INI--
opcache.jit=0101
opcache.jit_buffer_size=64M
--FILE--
<?php
echo match (random_int(1, 2)) {
    1, 2 => 'foo',
};
?>
--EXPECT--
foo
