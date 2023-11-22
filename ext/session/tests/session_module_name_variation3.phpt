--TEST--
Test session_module_name() function : variation
--INI--
session.save_path=
session.name=PHPSESSID
session.save_handler=files
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_module_name() : variation ***\n";

class MySessionHandler implements SessionHandlerInterface {
    public function open($save_path, $session_name): bool {
        throw new Exception("Stop...!");
    }
    public function close(): bool { return true; }
    public function read($id): string|false { return ''; }
    public function write($id, $session_data): bool { return true; }
    public function destroy($id): bool { return true; }
    public function gc($maxlifetime): int { return 1; }
}

var_dump(session_module_name("files"));
session_set_save_handler(new MySessionHandler());
var_dump(session_module_name());
var_dump(session_start());
var_dump(session_module_name());
var_dump(session_destroy());

ob_end_flush();
?>
--EXPECTF--
*** Testing session_module_name() : variation ***
string(5) "files"
string(4) "user"

Fatal error: Uncaught Exception: Stop...! in %s:%d
Stack trace:
#0 [internal function]: MySessionHandler->open('', 'PHPSESSID')
#1 %s(%d): session_start()
#2 {main}
  thrown in %s on line %d
