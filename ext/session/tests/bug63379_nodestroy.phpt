--TEST--
Bug #63379: Warning when using session_regenerate_id(TRUE) with a SessionHandler
--INI--
session.save_handler=files
session.name=PHPSESSID
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

$handler = new SessionHandler;
session_set_save_handler($handler);

session_start();

$_SESSION['foo'] = 'hello';
var_dump($_SESSION);

session_regenerate_id(false);

echo "*** Regenerated ***\n";
var_dump($_SESSION);

$_SESSION['bar'] = 'world';

var_dump($_SESSION);

session_write_close();
session_unset();

session_start();
var_dump($_SESSION);
--EXPECT--
array(1) {
  ["foo"]=>
  string(5) "hello"
}
*** Regenerated ***
array(1) {
  ["foo"]=>
  string(5) "hello"
}
array(2) {
  ["foo"]=>
  string(5) "hello"
  ["bar"]=>
  string(5) "world"
}
array(2) {
  ["foo"]=>
  string(5) "hello"
  ["bar"]=>
  string(5) "world"
}
