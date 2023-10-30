--TEST--
JIT Match: 003 impossible input
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function foo(C $i) {
    match($i) {
    	1,2=>0
    };
};
?>
DONE
--EXPECT--
DONE
