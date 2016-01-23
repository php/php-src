--TEST--
session id by COOKIE
--INI--
session.hash_function=1
session.hash_bis_per_charactor=4
session.save_path=
session.name=PHPSESSID
session.use_strict_mode=0
session.use_cookies=1
session.use_only_cookies=1
--SKIPIF--
<?php include('skipif.inc'); ?>
--GET--
PHPSESSID=session-id-by-get
--COOKIE--

--FILE--
<?php
session_start();
var_dump(session_id());
var_dump($_GET, $_POST, $_COOKIE, $_ENV['REQUEST_METHOD'], $_SESSION, session_status() === PHP_SESSION_ACTIVE);
session_destroy(true);
?>
--EXPECTF--
string(40) "%s"
array(1) {
  ["PHPSESSID"]=>
  string(17) "session-id-by-get"
}
array(0) {
}
array(0) {
}
string(3) "GET"
array(0) {
}
bool(true)
