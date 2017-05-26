--TEST--
Bug #71843 (null ptr deref ZEND_RETURN_SPEC_CONST_HANDLER (zend_vm_execute.h:3479))
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0xFFFFBFFF
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache')) die("skip"); ?>
--FILE--
<?php
0 & ~E & ~R;
6 && ~See
?>
okey
--EXPECTF--
Warning: Use of undefined constant E - assumed 'E' (this will throw an Error in a future version of PHP) in %sbug71843.php on line %d

Warning: A non-numeric value encountered in %s on line %d

Warning: Use of undefined constant R - assumed 'R' (this will throw an Error in a future version of PHP) in %sbug71843.php on line %d

Warning: A non-numeric value encountered in %s on line %d

Warning: Use of undefined constant See - assumed 'See' (this will throw an Error in a future version of PHP) in %sbug71843.php on line %d
okey
