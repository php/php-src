--TEST--
Bug #72940 - SID always defined
--INI--
error_reporting=-1
session.save_path=
session.name=PHPSESSID
session.save_handler=files
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--COOKIE--
PHPSESSID=bug72940test
--GET--
PHPSESSID=bug72940get
--FILE--
<?php
ob_start();

ini_set('session.use_strict_mode', 0);
ini_set('session.use_cookies', 1);
ini_set('session.use_only_cookies', 0);
session_start();
var_dump(session_id(), SID);
session_destroy();

ini_set('session.use_strict_mode', 0);
ini_set('session.use_cookies', 0);
ini_set('session.use_only_cookies', 0);
session_start();
var_dump(session_id(), SID);
session_destroy();
?>
--EXPECT--
string(12) "bug72940test"
string(0) ""
string(11) "bug72940get"
string(21) "PHPSESSID=bug72940get"
