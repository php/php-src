--TEST--
Test session_set_save_handler() function: interface
--EXTENSIONS--
session
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() function: interface ***\n";

class MySession2 implements SessionHandlerInterface {
    public $path;

    public function open($path, $name): bool{
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
}

$handler = new MySession2;
session_set_save_handler(array($handler, 'open'), array($handler, 'close'),
    array($handler, 'read'), array($handler, 'write'), array($handler, 'destroy'), array($handler, 'gc'));
session_start();

$_SESSION['foo'] = "hello";

var_dump(session_id(), ini_get('session.save_handler'), $_SESSION);

session_write_close();
session_unset();

session_start();
var_dump($_SESSION);

session_write_close();
session_unset();

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
*** Testing session_set_save_handler() function: interface ***

Warning: Class MySession2 implementing SessionHandlerInterface is missing the create_sid() method which will be required in PHP 9.0 in %s on line %d

Warning: Class MySession2 implementing SessionHandlerInterface is missing the validateId() method which will be required in PHP 9.0 in %s on line %d

Deprecated: session_set_save_handler(): Providing individual callbacks instead of an object implementing SessionHandlerInterface is deprecated in %s on line %d
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

Deprecated: session_set_save_handler(): Providing an object to argument #1 ($sessionhandler) which does not have the create_sid() method defined is deprecated in %s on line %d

Deprecated: session_set_save_handler(): Providing an object to argument #1 ($sessionhandler) which does not have the validateId() method defined is deprecated in %s on line %d
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
