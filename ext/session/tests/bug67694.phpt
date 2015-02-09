--TEST--
Bug #67694 Regression in session_regenerate_id()
--INI--
session.save_handler = files
session.name=PHPSESSID
session.auto_start = 0
session.use_cookies = 0
session.cache_limiter =
date.timezone = UTC
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

$data = date('r');

session_start();
$id = session_id();
$_SESSION['init'] = $data;

session_write_close();

session_id($id);
session_start();

session_regenerate_id(false);

$newid = session_id();
var_dump($newid != $id);

session_write_close();

unset($_SESSION);

session_id($newid);
session_start();

var_dump(isset($_SESSION['init']) && $data == $_SESSION['init']);
--EXPECT--
bool(true)
bool(true)
