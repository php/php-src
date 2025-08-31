--TEST--
GH-15657 (Segmentation fault in ext/opcache/jit/ir/dynasm/dasm_x86.h)
--EXTENSIONS--
opcache
--INI--
opcache.jit_buffer_size=64M
opcache.jit=1101
--FILE--
<?php
// Triggering the inheritance cache via implementing this interface is important to reproduce the bug
interface I {}

class A implements I {
    private $_prop;
    public $prop {
        get => $this->_prop;
    }
}
echo "Done\n";
?>
--EXPECT--
Done
