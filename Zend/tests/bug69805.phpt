--TEST--
Bug #69805 (null ptr deref and seg fault in zend_resolve_class_name)
--FILE--
<?php
class p{public function c(){(0)::t;}}?>
?>
--EXPECTF--
Fatal error: Class name must be a string in %s on line %d
