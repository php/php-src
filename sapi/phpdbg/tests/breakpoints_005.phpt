--TEST--
Test breakpoint into function context
--SKIPIF--
<?php
if (ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('skip phpdbg is incompatible with JIT');
}
?>
--PHPDBG--
b breakpoints_005.php:4
r
ev $bar
c
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:4]
prompt> [Breakpoint #0 at %s:4, hits: 1]
>00004: 	var_dump($bar);
 00005: }
 00006: 
prompt> test
prompt> string(4) "test"
[Script ended normally]
prompt> 
--FILE--
<?php

function foo($bar) {
	var_dump($bar);
}

foo("test");
