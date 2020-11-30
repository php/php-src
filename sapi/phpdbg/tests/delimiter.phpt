--TEST--
Test # delimiter parsing and execution
--SKIPIF--
<?php
if (ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('skip phpdbg is incompatible with JIT');
}
?>
--PHPDBG--
ev 1 + 3 # ev 2 ** 3#q
--EXPECT--
prompt> 4
8
