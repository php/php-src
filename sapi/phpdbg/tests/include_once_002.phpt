--TEST--
include_once must include only once #2
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
prompt> 1
[Script ended normally]
prompt> 
--FILE--
<?php

include __DIR__.'/include.inc';
include_once __DIR__.'/include.inc';
