--TEST--
JIT ASSIGN: incorrect narrowing to double
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
function test(){
	$x = (object)['x'=>0];
	for($i=0;$i<10;$i++){
		+$a;
		$a=$x->x;
		$a=7;
	}
}
test()
?>
DONE
--EXPECTF--
Warning: Undefined variable $a in %sassign_047.php on line 5
DONE

