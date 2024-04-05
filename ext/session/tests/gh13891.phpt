--TEST--
GH-13891 (memleak and segfault when using ini_set with session.trans_sid_hosts)
--INI--
session.use_cookies=0
session.use_only_cookies=0
session.use_trans_sid=1
session.trans_sid_hosts=php.net
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
// We *must* set it here because the bug only triggers on a runtime edit
ini_set('session.trans_sid_hosts','php.net');
?>
--EXPECT--
