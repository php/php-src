--TEST--
JIT RECV: infinite loop
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
;opcache.jit_debug=257
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function test(array $args, $short_options, $long_options = null)
{
    echo "okey";
}

test(array(), "d:e", 222, 3434);

?>
--EXPECT--
okey
