--TEST--
JIT ASSIGN_DIM_OP: overflow
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
class test {
    public$member;
    function __construct(){
        $this->member = 9223372036854775807;
        $this->member += 1;
    }
}
new test();
?>
DONE
--EXPECT--
DONE
