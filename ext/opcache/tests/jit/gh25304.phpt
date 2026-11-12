--TEST--
GH-25304 (JIT: SEGV in exception handler stub after OOM inside an output-buffer callback)
--CREDITS--
YuanchengJiang
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--INI--
memory_limit=16M
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
--FILE--
<?php
$o = new stdClass;
ob_start(function ($buffer) use (&$storage) {
    $storage .= $buffer;
    return '';
}, 1);
while ($o < 10);
?>
--EXPECTF--
Notice: Object of class stdClass could not be converted to int in %s on line %d

Fatal error: Allowed memory size of %d bytes exhausted%S(tried to allocate %d bytes) in %s on line %d
