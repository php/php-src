--TEST--
GH-15661 (Access null pointer in Zend/Optimizer/zend_inference.c)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1201
opcache.jit_buffer_size=64M
--FILE--
<?php

function test() {
    require 'dummy.inc';
    global $foo, $bar;
    $foo |= $bar;
}

echo "Done\n";

?>
--EXPECT--
Done
