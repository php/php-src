--TEST--
session rfc1867
--INI--
file_uploads=1
upload_max_filesize=1024
session.save_path=
session.name=PHPSESSID
session.use_strict_mode=0
session.use_cookies=1
session.use_only_cookies=0
session.upload_progress.enabled=1
session.upload_progress.cleanup=0
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
session.upload_progress.freq=1%
session.upload_progress.min_freq=0.000000001
session.save_handler=files
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--COOKIE--
PHPSESSID=rfc1867-inter
--GET--
PHPSESSID=rfc1867-inter-get
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHPSESSID"

rfc1867-inter-post
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

rfc1867_inter.php_1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"

1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

rfc1867_inter.php_2
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file2"; filename="file2.txt"

2
-----------------------------20896060251896012921717172737--
--FILE--
<?php
session_start();
var_dump(session_id());
var_dump($_FILES);
var_dump($_SESSION["upload_progress_" . basename(__FILE__) . "_1"]);
var_dump(isset($_SESSION["upload_progress_" . basename(__FILE__) . "_2"]));
session_destroy();
?>
--EXPECTF--
string(%d) "rfc1867-inter"
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
array(5) {
  ["start_time"]=>
  int(%d)
  ["content_length"]=>
  int(%d)
  ["bytes_processed"]=>
  int(%d)
  ["done"]=>
  bool(true)
  ["files"]=>
  array(2) {
    [0]=>
    array(7) {
      ["field_name"]=>
      string(5) "file1"
      ["name"]=>
      string(9) "file1.txt"
      ["tmp_name"]=>
      string(%d) "%s"
      ["error"]=>
      int(0)
      ["done"]=>
      bool(true)
      ["start_time"]=>
      int(%d)
      ["bytes_processed"]=>
      int(1)
    }
    [1]=>
    array(7) {
      ["field_name"]=>
      string(5) "file2"
      ["name"]=>
      string(9) "file2.txt"
      ["tmp_name"]=>
      string(%d) "%s"
      ["error"]=>
      int(0)
      ["done"]=>
      bool(true)
      ["start_time"]=>
      int(%d)
      ["bytes_processed"]=>
      int(1)
    }
  }
}
bool(false)
