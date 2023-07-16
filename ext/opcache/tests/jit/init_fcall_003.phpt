--TEST--
JIT INIT_FCALL: 003 incorrect init fcall guard (fail with tracing JIT and --repeat 3)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit_max_polymorphic_calls=0
opcache.jit=tracing
opcache.jit_hot_loop=64
opcache.jit_hot_func=127
opcache.jit_hot_return=8
opcache.jit_hot_side_exit=8
--EXTENSIONS--
opcache
--FILE--
<?php
include(__DIR__ . '/init_fcall_003.inc');
for($a=1; $a<100; $a++){
    f('1');
    f('1');
    f('1');
}
touch(__DIR__ . '/init_fcall_003.inc');
opcache_invalidate(__DIR__ . '/init_fcall_003.inc', true);
?>
DONE
--EXPECT--
DONE
