--TEST--
Test session_set_save_handler() function: class with create_sid
--INI--
session.save_handler=files
session.name=PHPSESSID
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() function: class with create_sid ***\n";

class MySession2 extends SessionHandler {
    public $path;

    public function open($path, $name): bool {
        if (!$path) {
            $path = sys_get_temp_dir();
        }
        $this->path = $path . '/u_sess_' . $name;
        return true;
    }

    public function close(): bool {
        return true;
    }

    public function read($id): string|false {
        return (string)@file_get_contents($this->path . $id);
    }

    public function write($id, $data): bool {
        return (bool)file_put_contents($this->path . $id, $data);
    }

    public function destroy($id): bool {
        @unlink($this->path . $id);
    }

    public function gc($maxlifetime): int|false {
        foreach (glob($this->path . '*') as $filename) {
            if (filemtime($filename) + $maxlifetime < time()) {
                @unlink($filename);
            }
        }
        return true;
    }

    public function create_sid(): string {
        return parent::create_sid();
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

session_write_close();
session_unset();
?>
--EXPECTF--
*** Testing session_set_save_handler() function: class with create_sid ***
string(%d) "%s"
string(4) "user"
array(1) {
  ["foo"]=>
  string(5) "hello"
}
array(1) {
  ["foo"]=>
  string(5) "hello"
}
