--TEST--
sqlite, session destroy test
--CREDITS--
Mats Lindh <mats at lindh.no>
#Testfest php.no
--INI--
session.save_handler = sqlite
--SKIPIF--
if (!extension_loaded("session"))
{
	die("skip Session module not loaded");
}
if (!extension_loaded("sqlite"))
{
	die("skip sqlite module not loaded");
}
--FILE--
<?php
/* Description: Tests that sqlite will destroy a session when used as a session handler
* Source code: ext/sqlite/sess_sqlite.c
*/
ob_start();
session_save_path(__DIR__ . "/sessiondb.sdb");

// start a session and save a value to it before commiting the session to the database
session_start();
$_SESSION["test"] = "foo_bar";
session_write_close();

// remove the session value
unset($_SESSION["test"]);
var_dump(isset($_SESSION["test"]));

// start the session again and destroy it
session_start();
var_dump($_SESSION["test"]);
session_destroy();
session_write_close();

unset($_SESSION["test"]);

// check that the session has been destroyed
session_start();
var_dump(isset($_SESSION["test"]));
ob_end_flush();
?>
--EXPECTF--
bool(false)
%unicode|string%(7) "foo_bar"
bool(false)
--CLEAN--
<?php
	unlink(__DIR__ . "/sessiondb.sdb")
?>
