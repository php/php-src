--TEST--
Test session_save_path() function : variation
--SKIPIF--
<?php include('skipif.inc');
if(substr(PHP_OS, 0, 3) == "WIN")
	die("skip Not for Windows");
?>
--INI--
session.save_handler=files
session.save_path=
session.name=PHPSESSID
--FILE--
<?php

ob_start();
/*
 * Prototype : string session_save_path([string $path])
 * Description : Get and/or set the current session save path
 * Source code : ext/session/session.c
 */

echo "*** Testing session_save_path() : variation ***\n";
$directory = __DIR__;
$sessions = ($directory."/sessions");

chdir($directory);
ini_set('open_basedir', '.');
// Delete the existing directory
if (file_exists($sessions) === TRUE) {
    @rmdir($sessions);
}

var_dump(mkdir($sessions));
var_dump(chdir($sessions));
ini_set("session.save_path", $directory);
var_dump(session_save_path());
var_dump(rmdir($sessions));

echo "Done";
ob_end_flush();
?>
--CLEAN--
$directory = __DIR__;
$sessions = ($directory."/sessions");
var_dump(rmdir($sessions));
--EXPECTF--
*** Testing session_save_path() : variation ***
bool(true)
bool(true)

Warning: ini_set(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (.) in %s on line %d
string(0) ""
bool(true)
Done
