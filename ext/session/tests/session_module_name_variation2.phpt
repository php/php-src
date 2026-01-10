--TEST--
Test session_module_name() function : variation
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_module_name() : variation ***\n";

class MySessionHandler implements SessionHandlerInterface {
    public function open($save_path, $session_name): bool { return false; }
    public function close(): bool { return false; }
    public function read($id): string|false { return false; }
    public function write($id, $session_data): bool { return false; }
    public function destroy($id): bool { return false; }
    public function gc($maxlifetime): int { return 1; }
}

var_dump(session_module_name("files"));
session_set_save_handler(new MySessionHandler());
var_dump(session_module_name());

ob_end_flush();
?>
--EXPECTF--
*** Testing session_module_name() : variation ***
string(%d) "%s"
string(4) "user"
