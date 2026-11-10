--TEST--
GH-24397 (JIT: stale label operand on a MERGE reduced to BEGIN)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=function
opcache.jit_debug=0x1000000
--EXTENSIONS--
opcache
--FILE--
<?php
function f() {
    foreach ([1, 2] as $v) {
        $c[] = $v;
    }
}
f();
echo "Done\n";
?>
--EXPECTF--
%ADone
