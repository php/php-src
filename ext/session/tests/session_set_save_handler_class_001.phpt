--TEST--
Test session_set_save_handler() : basic class wrapping existing handler
--INI--
session.use_strict_mode=1
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
	public function create_sid() {
		++$this->i;
		echo 'Create SID ', session_id(), "\n";
		return parent::create_sid();
	}
	public function validateSid($key) {
		++$this->i;
		echo 'Validate SID ', session_id(), "\n";
		return parent::validateSid($key);
	}
	public function read($key) {
		++$this->i;
		echo 'Read ', session_id(), "\n";
		return parent::read($key);
	}
	public function write($key, $data) {
		++$this->i;
		echo 'Write ', session_id(), "\n";
		return parent::write($key, $data);
	}
	public function updateTimestamp($key, $data) {
		++$this->i;
		echo 'Write ', session_id(), "\n";
		return parent::updateTimestamp($key, $data);
	}
	public function close() {
		++$this->i;
		echo 'Close ', session_id(), "\n";
		return parent::close();
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
Create SID 
Validate SID %s
Read %s
string(%d) "%s"
string(5) "files"
string(4) "user"
int(4)
array(0) {
}
Write %s
Close %s
Open %s
Validate SID %s
Read %s
array(1) {
  ["foo"]=>
  string(5) "hello"
}
Write %s
Close %s
int(11)
