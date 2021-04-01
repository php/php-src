--TEST--
JIT ASSIGN: 033
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo()
{
    $mode = 0;
    $working = 0;
    while ($mode == 0) {
        $working = $mode = 1;
    }
}
echo "ok\n";
?>
--EXPECT--
ok
