--TEST--
Test session_set_save_handler() : basic class wrapping existing handler
--INI--
session.use_strict_mode=1
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

class MySession implements SessionSaveHandlerInterface {
	public $i = 0;
	public function open($path, $name) {
		++$this->i;
		echo 'Open ', session_id(), "\n";
		return TRUE;
	}
	public function read($key) {
		++$this->i;
		echo 'Read ', session_id(), "\n";
		return "";
	}
	public function write($key, $data) {
		++$this->i;
		echo 'Write ', session_id(), "\n";
		return TRUE;
	}
	public function close() {
		++$this->i;
		echo 'Close ', session_id(), "\n";
		return TRUE;
	}
	public function destroy($key) {
		++$this->i;
		return TRUE;
	}
	public function gc($key) {
		return 0;
	}
	public function createId() {
		++$this->i;
		echo 'New Create ID ', session_id(), "\n";
		return "1234567890";
	}
	public function validateId($key) {
		++$this->i;
		echo 'Validate ID ', session_id(), "\n";
		return TRUE;
	}
	public function updateTimestamp($key, $data) {
		++$this->i;
		echo 'Update Timestamp ', session_id(), "\n";
		return TRUE;
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
var_dump($handler->i);

--EXPECTF--
*** Testing session_set_save_handler() : basic class wrapping existing handler ***
Open 
New Create ID 
Read 1234567890
string(10) "1234567890"
string(5) "files"
string(4) "user"
int(3)
array(0) {
}
Write 1234567890
Close 1234567890
Open 1234567890
Validate ID 1234567890
Read 1234567890
array(0) {
}
Write 1234567890
Close 1234567890
int(10)
