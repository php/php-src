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
Notice: Use of undefined constant E - assumed 'E' in %sbug71843.php on line %d

Notice: Use of undefined constant R - assumed 'R' in %sbug71843.php on line %d

Notice: Use of undefined constant See - assumed 'See' in %sbug71843.php on line %d
okey
