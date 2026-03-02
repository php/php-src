--TEST--
oss-fuzz #65498
--INI--
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php
function test() {
    for ($cnt=0; $cnt<6; $cnt++)
        for(;$y;y);
}
?>
DONE
--EXPECT--
DONE
