--TEST--
JIT: FETCH_OBJ 005
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit=tracing
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
for ($i = 0; $i < 3; $i++) {
	$a =& $b;
	$a->p;
}
?>
--EXPECTF--
Warning: Attempt to read property "p" on null in %sfetch_obj_005.php on line 4

Warning: Attempt to read property "p" on null in %sfetch_obj_005.php on line 4

Warning: Attempt to read property "p" on null in %sfetch_obj_005.php on line 4