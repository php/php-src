--TEST--
Test session_set_save_handler() function: interface wrong
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

echo "*** Testing session_set_save_handler() function: interface wrong ***\n";

interface MySessionHandlerInterface {
    public function open($path, $name): bool;
    public function close(): bool;
    public function read($id): string|false;
    public function write($id, $data): bool;
    public function destroy($id): bool;
    public function gc($maxlifetime): int|false;
}

class MySession2 implements MySessionHandlerInterface {
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
        echo "Unsupported session handler in use\n";
        return false;
    }

    public function destroy($id): bool {
        @unlink($this->path . $id);
        return true;
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

function good_write($id, $data) {
    global $handler;
    echo "good handler writing\n";
    return file_put_contents($handler->path . $id, $data);
}

$handler = new MySession2;

$ret = session_set_save_handler(array($handler, 'open'), array($handler, 'close'),
    array($handler, 'read'), 'good_write', array($handler, 'destroy'), array($handler, 'gc'));

var_dump($ret);
try {
    $ret = session_set_save_handler($handler);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

session_start();
?>
--EXPECT--
*** Testing session_set_save_handler() function: interface wrong ***
bool(true)
session_set_save_handler(): Argument #1 ($open) must be of type SessionHandlerInterface, MySession2 given
good handler writing

Deprecated: PHP Request Shutdown: Session callback must have a return value of type bool, int returned in Unknown on line 0
