--TEST--
Test session_set_save_handler() : inheritance
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

echo "*** Testing session_set_save_handler() : inheritance ***\n";

class MySession3 extends SessionHandler {
	public $i = 0;
	public function open($path, $name) {
		++$this->i;
		return parent::open($path, $name);
	}
	public function read($key) {
		++$this->i;
		return parent::read($key);
	}
}

class MySession4 extends MySession3 {
	public function write($id, $data) {
		$this->i = "hai";
		return parent::write($id, $data);
	}
}

$handler = new MySession3;
session_set_save_handler($handler);
session_start();

$_SESSION['foo'] = "hello";

session_write_close();
session_unset();

session_start();

var_dump($_SESSION, $handler->i);

session_write_close();
session_unset();

$handler = new MySession4;
session_set_save_handler($handler);

session_start();

$_SESSION['bar'] = 'hello';
session_write_close();
session_unset();

var_dump(session_id(), $_SESSION, $handler->i);
--EXPECTF--
*** Testing session_set_save_handler() : inheritance ***
array(1) {
  ["foo"]=>
  string(5) "hello"
}
int(4)
string(%d) "%s"
array(2) {
  ["foo"]=>
  string(5) "hello"
  ["bar"]=>
  string(5) "hello"
}
string(3) "hai"
