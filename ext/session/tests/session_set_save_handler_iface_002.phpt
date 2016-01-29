--TEST--
Test session_set_save_handler() function: interface wrong
--INI--
session.save_handler=files
session.name=PHPSESSID
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_set_save_handler(SessionHandlerInterface $handler [, bool $register_shutdown_function = true])
 * Description : Sets user-level session storage functions
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_set_save_handler() function: interface wrong ***\n";

interface MySessionHandlerInterface {
	public function open($path, $name);
	public function close();
	public function read($id);
	public function write($id, $data);
	public function destroy($id);
	public function gc($maxlifetime);
}

class MySession2 implements MySessionHandlerInterface {
	public $path;

	public function open($path, $name) {
		if (!$path) {
			$path = sys_get_temp_dir();
		}
		$this->path = $path . '/u_sess_' . $name;
		return true;
	}

	public function close() {
		return true;
	}

	public function read($id) {
		return @file_get_contents($this->path . $id);
	}

	public function write($id, $data) {
		echo "Unsupported session handler in use\n";
	}

	public function destroy($id) {
		@unlink($this->path . $id);
	}

	public function gc($maxlifetime) {
		foreach (glob($this->path . '*') as $filename) {
			if (filemtime($filename) + $maxlifetime < time()) {
				@unlink($filename);
			}
		}
		return true;
	}
}

function good_write($id, $data) {
	global $handler;
	echo "good handler writing\n";
	return file_put_contents($handler->path . $id, $data);
}

$handler = new MySession2;

$ret = session_set_save_handler(array($handler, 'open'), array($handler, 'close'),
	array($handler, 'read'), 'good_write', array($handler, 'destroy'), array($handler, 'gc'));

var_dump($ret);
$ret = session_set_save_handler($handler);
var_dump($ret);

session_start();

--EXPECTF--
*** Testing session_set_save_handler() function: interface wrong ***
bool(true)

Warning: session_set_save_handler() expects parameter 1 to be SessionHandlerInterface, object given in %s
bool(false)
good handler writing
