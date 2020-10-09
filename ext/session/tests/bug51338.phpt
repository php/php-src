--TEST--
Bug #51338 (URL-Rewriter should not get enabled if use_only_cookies is set to 1)
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_only_cookies=1
session.use_trans_sid=1
--FILE--
<?php
session_start();
print_r(ob_list_handlers());
?>
--EXPECT--
Array
(
)
