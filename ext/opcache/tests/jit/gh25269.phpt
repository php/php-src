--TEST--
GH-25269 (JIT: misaligned zval passed to ZEND_ADD_ARRAY_ELEMENT handler)
--CREDITS--
YuanchengJiang
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=function
--FILE--
<?php
set_error_handler(function($errno, $errstr) {
    throw new ErrorException($errstr);
});
function test() {
    return [null => 1, "a" => 2, "b" => 3];
}
try {
    test();
} catch (ErrorException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Using null as an array offset is deprecated, use an empty string instead
