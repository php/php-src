--TEST--
session id by GET
--INI--
session.save_path=
session.name=PHPSESSID
session.use_strict_mode=0
session.use_cookies=0
session.use_only_cookies=0
--SKIPIF--
<?php include('skipif.inc'); ?>
--GET--
PHPSESSID=session-id-by-get
--COOKIE--
PHPSESSID=session-id-by-cookie
--FILE--
<?php
session_start();
var_dump(session_id());
var_dump($_GET, $_POST, $_COOKIE, $_ENV['REQUEST_METHOD'], $_SESSION, session_status() === PHP_SESSION_ACTIVE);
@session_destroy(true);
?>
--EXPECTF--
string(17) "session-id-by-get"
array(1) {
  ["PHPSESSID"]=>
  string(17) "session-id-by-get"
}
array(0) {
}
array(1) {
  ["PHPSESSID"]=>
  string(20) "session-id-by-cookie"
}
string(3) "GET"
array(0) {
}
bool(true)
