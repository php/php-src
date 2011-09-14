--TEST--
Test session_set_save_handler() : basic class wrapping existing handler
--INI--
session.save_handler=files
session.name=PHPSESSID
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_set_save_handler(SessionHandler $handler [, bool $register_shutdown_function = true])
 * Description : Sets user-level session storage functions
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_set_save_handler() : basic class wrapping existing handler ***\n";

class MySession extends SessionHandler {
	public $i = 0;
	public function open($path, $name) {
		++$this->i;
		echo 'Open ', session_id(), "\n";
		return parent::open($path, $name);
	}
	public function read($key) {
		++$this->i;
		echo 'Read ', session_id(), "\n";
		return parent::read($key);
	}
}

$oldHandler = ini_get('session.save_handler');
$handler = new MySession;
session_set_save_handler($handler);
session_start();

var_dump(session_id(), $oldHandler, ini_get('session.save_handler'), $handler->i, $_SESSION);

$_SESSION['foo'] = "hello";

session_write_close();
session_unset();

session_start();
var_dump($_SESSION);

session_write_close();
session_unset();

--EXPECTF--
*** Testing session_set_save_handler() : basic class wrapping existing handler ***
Open 
Read %s
string(%d) "%s"
string(5) "files"
string(4) "user"
int(2)
array(0) {
}
Open %s
Read %s
array(1) {
  ["foo"]=>
  string(5) "hello"
}
