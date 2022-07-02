--TEST--
Test session_set_save_handler() function: create_sid
--INI--
session.save_handler=files
session.name=PHPSESSID
session.save_path="{TMP}"
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() function: create_sid ***\n";

class MySession2 {
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
        return @file_get_contents($this->path . $id);
    }

    public function write($id, $data): bool {
        return file_put_contents($this->path . $id, $data);
    }

    public function destroy($id): bool {
        @unlink($this->path . $id);
        return false;
    }

    public function gc($maxlifetime): int|false {
        foreach (glob($this->path . '*') as $filename) {
            if (filemtime($filename) + $maxlifetime < time()) {
                @unlink($filename);
            }
        }
        return true;
    }

    #[ReturnTypeWillChange]
    public function create_sid() {
        return false;
    }
}

$handler = new MySession2;
session_set_save_handler(array($handler, 'open'), array($handler, 'close'),
    array($handler, 'read'), array($handler, 'write'), array($handler, 'destroy'), array($handler, 'gc'), array($handler, 'create_sid'));
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
*** Testing session_set_save_handler() function: create_sid ***

Fatal error: Uncaught Error: Session id must be a string in %s:%d
Stack trace:
#0 %s(%d): session_start()
#1 {main}
  thrown in %s on line %d
