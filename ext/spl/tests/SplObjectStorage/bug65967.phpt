--TEST--
Bug #65967: SplObjectStorage contains corrupt member variables after garbage collection
--INI--
zend.enable_gc=1
--FILE--
<?php
$objstore = new SplObjectStorage();
gc_collect_cycles();

var_export($objstore);
?>
--EXPECT--
\SplObjectStorage::__set_state(array(
))
