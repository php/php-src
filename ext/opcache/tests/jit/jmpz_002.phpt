--TEST--
JIT JMPZ: Separate JMPZ for "smart branch" may be only emitted by function JIT
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function test($b) {
    if ($b ? 0 : (X>0)){
        echo "Not taken\n";
    }
}
test(true); 
?>
DONE
--EXPECT--
DONE
