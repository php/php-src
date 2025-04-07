--TEST--
GH-15909 (Core dumped in Core dumped in ext/opcache/jit/ir/ir_cfg.c)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1001
opcache.jit_buffer_size=64M
--FILE--
<?php
function foo() {
for(;x;)for(;$fusion=1;);
}
?>
DONE
--EXPECT--
DONE
