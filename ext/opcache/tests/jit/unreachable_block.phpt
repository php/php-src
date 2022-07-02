--TEST--
JIT unreachable_block with block order
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
class A {
    public function Value()
    {
        switch ($this->returnType) {
            case 'float':  return $this->returnTypeNullable ? null : 0;
            default: return;
        }
    }
}
?>
okey
--EXPECT--
okey
