--TEST--
Bug #73973 debug_zval_dump() assertion error for resource consts with --enable-debug
--FILE--
<?php
define('myerr', fopen('php://stderr', 'w'));
debug_zval_dump(myerr);
?>
--EXPECTF--
resource(5) of type (stream) refcount(%d)
