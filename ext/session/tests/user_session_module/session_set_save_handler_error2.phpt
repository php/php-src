--TEST--
Test session_set_save_handler() function : error functionality
--EXTENSIONS--
session
--INI--
error_reporting=0
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : error functionality ***\n";

class MySessionHandler implements SessionHandlerInterface {
    function open($save_path, $session_name): bool { return true; }
    function close(): bool { return true; }
    function read($id): string|false { return false; }
    function write($id, $session_data): bool { }
    function destroy($id): bool {  return true; }
    function gc($maxlifetime): int|false {  return true; }
}

session_set_save_handler(new MySessionHandler());

session_start();
$_SESSION["Blah"] = "Hello World!";
$_SESSION["Foo"] = FALSE;
$_SESSION["Guff"] = 1234567890;
var_dump($_SESSION);

session_write_close();
var_dump($_SESSION);
session_set_save_handler(new MySessionHandler());
session_start();
var_dump($_SESSION);
session_destroy();

ob_end_flush();
?>
--EXPECT--
*** Testing session_set_save_handler() : error functionality ***
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
array(0) {
}
