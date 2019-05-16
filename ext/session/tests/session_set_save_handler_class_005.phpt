--TEST--
Test session_set_save_handler() : incomplete implementation
--INI--
session.save_handler=files
session.name=PHPSESSID
session.gc_probability=0
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

echo "*** Testing session_set_save_handler() : incomplete implementation ***\n";

class MySession6 extends SessionHandler {
	public function open($path, $name) {
		// don't call parent
		return true;
	}

	public function read($id) {
		// should error because parent::open hasn't been called
		return parent::read($id);
	}
}

$handler = new MySession6;
session_set_save_handler($handler);
var_dump(session_start());

var_dump(session_id(), ini_get('session.save_handler'), $_SESSION);

session_write_close();
session_unset();
--EXPECTF--
*** Testing session_set_save_handler() : incomplete implementation ***

Warning: SessionHandler::read(): Parent session handler is not open in %ssession_set_save_handler_class_005.php on line %d

Warning: SessionHandler::close(): Parent session handler is not open in %ssession_set_save_handler_class_005.php on line %d

Warning: session_start(): Failed to read session data: user (%s) in %ssession_set_save_handler_class_005.php on line %d
bool(false)
string(0) ""
string(4) "user"
array(0) {
}
