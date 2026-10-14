--TEST--
SID constant type confusion
--EXTENSIONS--
session
--SKIPIF--
<?php include 'skipif.inc'; ?>
--INI--
session.use_cookies=0
--FILE--
<?php

define('SID', [0xdeadbeef]);
session_start();
var_dump(SID);

?>
--EXPECT--
string(0) ""
