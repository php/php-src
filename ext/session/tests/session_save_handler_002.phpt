--TEST--
Test session_set_save_handler() : basic class wrapping existing handler
--INI--
session.use_strict_mode=1
session.save_handler=files
session.name=PHPSESSID
session.save_path=
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

require 'save_handler.inc';

session_save_path(__DIR__);

class MySession implements SessionSaveHandlerInterface {
	public $i = 0;
	public function open($path, $name) {
		++$this->i;
		return open($path, $name);
	}
	public function read($key) {
		++$this->i;
		return read($key);
	}
	public function write($key, $data) {
		++$this->i;
		return write($key, $data);
	}
	public function close() {
		++$this->i;
		return close();
	}
	public function destroy($key) {
		++$this->i;
		return destroy($key);
	}
	public function gc($key) {
		return gc($key);
	}
	public function createId() {
		++$this->i;
		return session_create_id();
	}
	public function validateId($key) {
		++$this->i;
		return validate_sid($key);
	}
	public function updateTimestamp($key, $data) {
		++$this->i;
		return update($key, $data);
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

session_start();
session_destroy();

--EXPECTF--
*** Testing session_set_save_handler() : basic class wrapping existing handler ***

Open [%s,PHPSESSID]
Read [%s,%s]
string(32) "%s"
string(5) "files"
string(4) "user"
int(3)
array(0) {
}
Write [%s,%s,foo|s:5:"hello";]
Close [%s,PHPSESSID]
Open [%s,PHPSESSID]
ValidateID [%s,%s]
Read [%s,%s]
array(1) {
  ["foo"]=>
  string(5) "hello"
}
Update [%s,%s]
Close [%s,PHPSESSID]
int(10)
Open [%s,PHPSESSID]
ValidateID [%s,%s]
Read [%s,%s]
Destroy [%s,%s]
Close [%s,PHPSESSID]
