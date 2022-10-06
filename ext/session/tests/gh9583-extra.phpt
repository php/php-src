--TEST--
GH-9583: session_create_id() fails with user defined save handler that doesn't have a validateId() method
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

class SessionHandlerTester implements \SessionHandlerInterface
{

    public function close(): bool { return true; }

    public function destroy($id): bool { return true; }

    public function gc($max_lifetime): int|false { return 1; }

    public function open($path, $name): bool { return true; }

    public function read($id): string { return ''; }

    public function write($id, $data): bool { return true; }

    //public function create_sid() { return uniqid(); }

    //public function validateId($key) { return true; }
}

$obj = new SessionHandlerTester();
ini_set('session.use_strict_mode','1');
session_set_save_handler($obj);
session_start();

$originalSessionId = session_id();

session_write_close();

session_start();

$newSessionId = session_id();

echo 'validateId() ', (method_exists($obj, 'validateId') ? ('returns ' . ($obj->validateId(1) ? 'true' : 'false')) : 'is commented out'), "\n";
var_dump($originalSessionId == $newSessionId);

?>
--EXPECT--
validateId() is commented out
bool(true)
