--TEST--
OSS-Fuzz #546798343 (Heap-buffer-overflow in zend_delete_call_instructions with callable conversion)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

$x = function() {};
gonnaBeInlined($x(...));
function gonnaBeInlined($foo) {
}

echo "Done";
?>
--EXPECT--
Done
