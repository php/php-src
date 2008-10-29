--TEST--
sqlite, session storage test
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
	die("skip Session module not loaded");
}
--FILE--
<?php
/* Description: Tests that sqlite can be used as a session save handler
* Source code: ext/sqlite/sess_sqlite.c
*/

ob_start();
session_save_path(__DIR__ . "/sessiondb.sdb");

// create the session and set a session value
session_start();
$_SESSION["test"] = "foo_bar";

// close the session and unset the value
session_write_close();
unset($_SESSION["test"]);
var_dump(isset($_SESSION["test"]));

// start the session again and check that we have the proper value
session_start();
var_dump($_SESSION["test"]);
ob_end_flush();
?>
--EXPECTF--
bool(false)
%unicode|string%(7) "foo_bar"
--CLEAN--
<?php
	unlink(__DIR__ . "/sessiondb.sdb")
?>
