--TEST--
Bug #71683 Null pointer dereference in zend_hash_str_find_bucket
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_handler=files
session.auto_start=1
session.use_only_cookies=0
--FILE--
<?php
ob_start();
echo "ok\n";
?>
--EXPECTF--
ok
