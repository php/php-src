--TEST--
Test session_set_save_handler() function: id interface
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

echo "*** Testing session_set_save_handler() function: id interface ***\n";

class MySession2 implements SessionHandlerInterface, SessionIdInterface {
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
        return (string)@file_get_contents($this->path . $id);
    }

    public function write($id, $data) {
        // Empty $data = 0 = false
        return (bool)file_put_contents($this->path . $id, $data);
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

    public function create_sid() {
        return pathinfo(__FILE__)['filename'];
    }
}

$handler = new MySession2;
session_set_save_handler($handler);
session_start();

$_SESSION['foo'] = "hello";

var_dump(session_id(), ini_get('session.save_handler'), $_SESSION);

session_write_close();
session_unset();

session_start();
var_dump($_SESSION);
--CLEAN--
<?php
@unlink(session_save_path().'/u_sess_PHPSESSIDsession_set_save_handler_iface_003');
--EXPECT--
*** Testing session_set_save_handler() function: id interface ***
string(34) "session_set_save_handler_iface_003"
string(4) "user"
array(1) {
  ["foo"]=>
  string(5) "hello"
}
array(1) {
  ["foo"]=>
  string(5) "hello"
}
