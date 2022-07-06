--TEST--
Bug #70782: null ptr deref and segfault (zend_get_class_fetch_type)
--FILE--
<?php

(-0)::$prop;

?>
--EXPECTF--
Fatal error: Illegal class name in %s on line %d
