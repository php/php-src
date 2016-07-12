--TEST--
Bug #71186 session.hash_function - algorithm changes
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.hash_function=sha512
session.save_handler=files
--FILE--
<?php
ob_start();
ini_set('session.use_strict_mode', 1);

session_start();
$orig = session_id();
session_regenerate_id();
$new = session_id();
var_dump(strlen($orig),strlen($new));
session_commit();

ini_set('session.hash_function','sha1');
session_id('invalid');
session_start();
$orig = session_id();
session_regenerate_id();
$new = session_id();
var_dump(strlen($orig),strlen($new));
?>
--EXPECT--
int(128)
int(128)
int(40)
int(40)
