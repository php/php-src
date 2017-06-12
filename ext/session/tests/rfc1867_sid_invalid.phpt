--TEST--
session rfc1867 sid cookie
--INI--
file_uploads=1
error_reporting=E_ALL&~E_NOTICE
comment=debug builds show some additional E_NOTICE errors
upload_max_filesize=1024
session.save_path=
session.name=PHPSESSID
session.use_cookies=1
session.use_only_cookies=0
session.use_strict_mode=0
session.auto_start=0
session.upload_progress.enabled=1
session.upload_progress.cleanup=0
session.upload_progress.prefix=upload_progress_
session.upload_progress.name=PHP_SESSION_UPLOAD_PROGRESS
session.upload_progress.freq=0
session.save_handler=files
--SKIPIF--
<?php include('skipif.inc'); ?>
--COOKIE--
PHPSESSID=_
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="PHP_SESSION_UPLOAD_PROGRESS"

rfc1867_sid_invalid.php
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"

1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file2"; filename="file2.txt"

2
-----------------------------20896060251896012921717172737--
--FILE--
<?php
error_reporting(0);
ob_start();
session_start();
var_dump(session_id());
var_dump(basename(__FILE__) == $_POST[ini_get("session.upload_progress.name")]);
var_dump($_FILES);
var_dump($_SESSION["upload_progress_" . basename(__FILE__)]);
session_destroy();
?>
--EXPECTF--
Warning: Unknown: The session id is too long or contains illegal characters, valid characters are a-z, A-Z, 0-9 and '-,' in Unknown on line 0

Warning: Unknown: Failed to read session data: files (path: ) in Unknown on line 0

Warning: Unknown: Failed to write session data (files). Please verify that the current setting of session.save_path is correct () in Unknown on line 0

Warning: Unknown: The session id is too long or contains illegal characters, valid characters are a-z, A-Z, 0-9 and '-,' in Unknown on line 0

Warning: Unknown: Failed to read session data: files (path: ) in Unknown on line 0

Warning: Unknown: Failed to write session data (files). Please verify that the current setting of session.save_path is correct () in Unknown on line 0
string(%d) ""
bool(true)
array(2) {
  ["file1"]=>
  array(5) {
    ["name"]=>
    %string|unicode%(9) "file1.txt"
    ["type"]=>
    %string|unicode%(0) ""
    ["tmp_name"]=>
    %string|unicode%(%d) "%s"
    ["error"]=>
    int(0)
    ["size"]=>
    int(1)
  }
  ["file2"]=>
  array(5) {
    ["name"]=>
    %string|unicode%(9) "file2.txt"
    ["type"]=>
    %string|unicode%(0) ""
    ["tmp_name"]=>
    %string|unicode%(%d) "%s"
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
  int(469)
  ["bytes_processed"]=>
  int(469)
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
