--TEST--
JIT FETCH_DIM_R: 011
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
set_error_handler(function() {
    $GLOBALS['a'] = 0;
});
$a = [$y];
($a[17604692317316877817]);
?>
DONE
--EXPECT--
DONE
