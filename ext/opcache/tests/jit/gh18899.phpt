--TEST--
GH-18899 (JIT function crash when emitting undefined variable warning and opline is not set yet)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1205
opcache.jit_buffer_size=8M
--FILE--
<?php
function ptr2str()
{
    for ($i=0; $i<8; $i++) {
        $ptr >>= 8;
    }
}
str_repeat("A",232).ptr2str();
?>
--EXPECTF--
Warning: Undefined variable $ptr in %s on line %d
