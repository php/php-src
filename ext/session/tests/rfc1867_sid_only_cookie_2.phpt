--TEST--
session rfc1867 sid only cookie 2
--INI--
upload_max_filesize=1024
session.upload_progress.cleanup=0
session.upload_progress.freq=0
--EXTENSIONS--
session
--SKIPIF--
<?php include 'skipif.inc'; ?>
--GET--
PHPSESSID=rfc1867-sid-only-cookie-2-get
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHPSESSID"

rfc1867-sid-only-cookie-2-post
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

rfc1867_sid_only_cookie_2.php
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"

1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file2"; filename="file2.txt"

2
-----------------------------20896060251896012921717172737--
--FILE--
<?php
session_start();
var_dump(session_id());
var_dump(basename(__FILE__) == $_POST[ini_get("session.upload_progress.name")]);
var_dump($_FILES);
var_dump(isset($_SESSION["upload_progress_" . basename(__FILE__)]));
session_destroy();
?>
--EXPECTF--
string(%d) "%s"
bool(true)
array(2) {
  ["file1"]=>
  array(6) {
    ["name"]=>
    string(9) "file1.txt"
    ["full_path"]=>
    string(9) "file1.txt"
    ["type"]=>
    string(0) ""
    ["tmp_name"]=>
    string(%d) "%s"
    ["error"]=>
    int(0)
    ["size"]=>
    int(1)
  }
  ["file2"]=>
  array(6) {
    ["name"]=>
    string(9) "file2.txt"
    ["full_path"]=>
    string(9) "file2.txt"
    ["type"]=>
    string(0) ""
    ["tmp_name"]=>
    string(%d) "%s"
    ["error"]=>
    int(0)
    ["size"]=>
    int(1)
  }
}
bool(false)
