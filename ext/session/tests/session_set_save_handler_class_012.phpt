--TEST--
Test session_set_save_handler() : incorrect arguments for existing handler open
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

echo "*** Testing session_set_save_handler() : incorrect arguments for existing handler open ***\n";

class MySession extends SessionHandler {
	public $i = 0;
	public function open($path, $name) {
		++$this->i;
		echo 'Open ', session_id(), "\n";
		// This test was written for broken return value handling
		// Mimmick what was actually being tested by returning true here
		return (null === parent::open());
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
var_dump(session_start());

var_dump(session_id(), $oldHandler, ini_get('session.save_handler'), $handler->i, $_SESSION);
--EXPECTF--
*** Testing session_set_save_handler() : incorrect arguments for existing handler open ***
Open 

Warning: SessionHandler::open() expects exactly 2 parameters, 0 given in %s on line %d
Read %s

Warning: SessionHandler::read(): Parent session handler is not open in %s on line %d

Warning: SessionHandler::close(): Parent session handler is not open in %s on line %d

Warning: session_start(): Failed to read session data: user (%s) in %s on line %d
bool(false)
string(0) ""
string(5) "files"
string(4) "user"
int(2)
array(0) {
}
