--TEST--
session rfc1867
--INI--
file_uploads=1
error_reporting=E_ALL&~E_NOTICE
comment=debug builds show some additional E_NOTICE errors
upload_max_filesize=1024
session.save_path=
session.name=PHPSESSID
session.use_cookies=1
session.use_only_cookies=0
session.upload_progress.enabled=1
session.upload_progress.cleanup=0
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
session.upload_progress.freq=1%
session.upload_progress.min_freq=0.000000001
--SKIPIF--
<?php include('skipif.inc'); ?>
--COOKIE--
PHPSESSID=rfc1867-tests
--GET--
PHPSESSID=rfc1867-tests-get
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHPSESSID"

rfc1867-tests-post
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

rfc1867.php
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
var_dump($_SESSION["upload_progress_" . basename(__FILE__)]);
session_destroy();
?>
--EXPECTF--
string(%d) "rfc1867-tests"
bool(true)
array(2) {
  [%u|b%"file1"]=>
  array(5) {
    [%u|b%"name"]=>
    %string|unicode%(9) "file1.txt"
    [%u|b%"type"]=>
    %string|unicode%(0) ""
    [%u|b%"tmp_name"]=>
    %string|unicode%(%d) "%s"
    [%u|b%"error"]=>
    int(0)
    [%u|b%"size"]=>
    int(1)
  }
  [%u|b%"file2"]=>
  array(5) {
    [%u|b%"name"]=>
    %string|unicode%(9) "file2.txt"
    [%u|b%"type"]=>
    %string|unicode%(0) ""
    [%u|b%"tmp_name"]=>
    %string|unicode%(%d) "%s"
    [%u|b%"error"]=>
    int(0)
    [%u|b%"size"]=>
    int(1)
  }
}
array(5) {
  [%u|b%"start_time"]=>
  int(%d)
  [%u|b%"content_length"]=>
  int(%d)
  [%u|b%"bytes_processed"]=>
  int(%d)
  [%u|b%"done"]=>
  bool(true)
  [%u|b%"files"]=>
  array(2) {
    [0]=>
    array(7) {
      [%u|b%"field_name"]=>
      %unicode|string%(5) "file1"
      [%u|b%"name"]=>
      %unicode|string%(9) "file1.txt"
      [%u|b%"tmp_name"]=>
      %unicode|string%(%d) "%s"
      [%u|b%"error"]=>
      int(0)
      [%u|b%"done"]=>
      bool(true)
      [%u|b%"start_time"]=>
      int(%d)
      [%u|b%"bytes_processed"]=>
      int(1)
    }
    [1]=>
    array(7) {
      [%u|b%"field_name"]=>
      %unicode|string%(5) "file2"
      [%u|b%"name"]=>
      %unicode|string%(9) "file2.txt"
      [%u|b%"tmp_name"]=>
      %unicode|string%(%d) "%s"
      [%u|b%"error"]=>
      int(0)
      [%u|b%"done"]=>
      bool(true)
      [%u|b%"start_time"]=>
      int(%d)
      [%u|b%"bytes_processed"]=>
      int(1)
    }
  }
}
