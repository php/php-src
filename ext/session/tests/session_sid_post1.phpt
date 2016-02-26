--TEST--
session id by POST
--INI--
session.save_path=
session.name=PHPSESSID
session.use_strict_mode=0
session.use_cookies=0
session.use_only_cookies=0
--SKIPIF--
<?php include('skipif.inc'); ?>
--GET--

--COOKIE--
PHPSESSID=session-id-by-cookie
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHPSESSID"

session-id-by-post
-----------------------------20896060251896012921717172737
--FILE--
<?php
session_start();
var_dump(session_id());
var_dump($_GET, $_POST, $_COOKIE, $_ENV['REQUEST_METHOD'], $_SESSION, session_status() === PHP_SESSION_ACTIVE);
session_destroy(true);
?>
--EXPECTF--
string(18) "session-id-by-post"
array(0) {
}
array(1) {
  ["PHPSESSID"]=>
  string(18) "session-id-by-post"
}
array(1) {
  ["PHPSESSID"]=>
  string(20) "session-id-by-cookie"
}
string(4) "POST"
array(0) {
}
bool(true)
