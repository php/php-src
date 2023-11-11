--TEST--
FETCH_R: 001 result reference counter may be decremented before use
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
$x = [&$v];
$y = 'x';
$$y == [&$x[0]];
?>
DONE
--EXPECT--
DONE
