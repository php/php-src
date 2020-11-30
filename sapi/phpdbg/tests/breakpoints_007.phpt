--TEST--
Basic method breakpoints
--SKIPIF--
<?php
if (ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('skip phpdbg is incompatible with JIT');
}
?>
--PHPDBG--
b bar::foo
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at bar::foo]
prompt> [Breakpoint #0 in bar::foo() at %s:5, hits: 1]
>00005: 		var_dump($bar);
 00006: 	}
 00007: }
prompt> 
--FILE--
<?php

class bar {
	function foo($bar) {
		var_dump($bar);
	}
}

(new bar)->foo("test");
