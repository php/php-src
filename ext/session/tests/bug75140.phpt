--TEST--
Bug #75140 (session_id max length 255 is too long with filesystem save handler)
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.sid_length=251
--FILE--
<?php

session_start();
echo "ok\n";

?>
--XFAIL--
See bug #75140
--EXPECT--
ok
