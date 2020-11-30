--TEST--
phpdbg_end_oplog() alone must not crash
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
prompt> 
Warning: Can not end an oplog without starting it in %s on line 3
NULL
[Script ended normally]
prompt> 
--FILE--
<?php

var_dump(phpdbg_end_oplog());
