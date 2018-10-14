--TEST--
Bug #61470 (session_regenerate_id() does not create session file)
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
--FILE--
<?php
ob_start();
ini_set('session.save_path', __DIR__);
$path = ini_get('session.save_path') . '/sess_';
session_start();
// starts session & creates and locks file
$file1 = $path . session_id();
var_dump(is_file($file1));

session_regenerate_id();
// starts new session, but file is not create!
$file2 = $path . session_id();
var_dump(is_file($file2));

// cleanup
@unlink($file1);
@unlink($file2);
--EXPECT--
bool(true)
bool(true)
