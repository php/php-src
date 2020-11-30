--TEST--
include()ing files should not raise "too many open files" error
--SKIPIF--
<?php
if (ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('skip phpdbg is incompatible with JIT');
}
?>
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Script ended normally]
prompt> 
--FILE--
<?php

for ($i = 0; $i < 25000; ++$i) {
    include __DIR__.'/empty.inc';
}
