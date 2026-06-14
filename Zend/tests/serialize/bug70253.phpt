--TEST--
Bug #70253 (segfault at _efree () in zend_alloc.c:1389)
--FILE--
<?php
unserialize('a:2:{i:0;O:9:"000000000":10000000');
?>
--EXPECTF--
Warning: unserialize(): Error at offset 33 of 33 bytes in %s on line %d
