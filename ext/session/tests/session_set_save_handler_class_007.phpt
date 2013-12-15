--TEST--
Test session_set_save_handler() : manual shutdown, reopen
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

echo "*** Testing session_set_save_handler() : manual shutdown, reopen ***\n";

class MySession extends SessionHandler {
	public $num;
	public function __construct($num) {
		$this->num = $num;
		echo "(#$this->num) constructor called\n";
	}
	public function __destruct() {
		echo "(#$this->num) destructor called\n";
	}
	public function finish() {
		$id = session_id();
		echo "(#$this->num) finish called $id\n";
		session_write_close();
	}
	public function write($id, $data) {
		echo "(#$this->num) writing $id = $data\n";
		return parent::write($id, $data);
	}
	public function close() {
		$id = session_id();
		echo "(#$this->num) closing $id\n";
		return parent::close();
	}
}

$handler = new MySession(1);
session_set_save_handler($handler);
session_start();

$_SESSION['foo'] = 'bar';

// explicit close
$handler->finish();

$handler = new MySession(2);
session_set_save_handler($handler);
session_start();

$_SESSION['abc'] = 'xyz';
// implicit close (called by shutdown function)
echo "done\n";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_save_handler() : manual shutdown, reopen ***
(#1) constructor called
(#1) finish called %s
(#1) writing %s = foo|s:3:"bar";
(#1) closing %s
(#2) constructor called
(#1) destructor called
done
(#2) writing %s = foo|s:3:"bar";abc|s:3:"xyz";
(#2) closing %s
(#2) destructor called
