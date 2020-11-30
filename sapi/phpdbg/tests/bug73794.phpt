--TEST--
Bug #73794 (Crash (out of memory) when using run and # command separator)
--SKIPIF--
<?php
if (ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('skip phpdbg is incompatible with JIT');
}
?>
--PHPDBG--
r echo # quit
--EXPECTF--
[Successful compilation of %s]
prompt> echo
--FILE--
<?php
echo $argv[1];
?>
