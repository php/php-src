--TEST--
Bug #61470 (session_regenerate_id() does not create session file)
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
--FILE--
<?php
$path = ini_get('session.save_path') . '/sess_';

ob_start();
session_start();
// starts session & creates and locks file
var_dump(is_file($path . session_id()));

session_regenerate_id();
// starts new session, but file is not create!
var_dump(is_file($path . session_id()));
--EXPECT--
bool(true);
bool(true);

