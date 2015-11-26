--TEST--
Bug #55500 (Corrupted $_FILES indices lead to security concern)
--INI--
file_uploads=1
error_reporting=E_ALL&~E_NOTICE
upload_max_filesize=1024
max_file_uploads=10
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file[]"; filename="file1.txt"
Content-Type: text/plain-file1

1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file[[type]"; filename="file2.txt"
Content-Type: text/plain-file2

2
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file[[name]"; filename="file3.txt"
Content-Type: text/plain-file3

3
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file[name]["; filename="file4.txt"
Content-Type: text/plain-file3

4
-----------------------------20896060251896012921717172737--
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
?>
--EXPECTF--
array(1) {
  [%u|b%"file"]=>
  array(5) {
    [%u|b%"name"]=>
    array(1) {
      [0]=>
      %unicode|string%(9) "file1.txt"
    }
    [%u|b%"type"]=>
    array(1) {
      [0]=>
      %unicode|string%(16) "text/plain-file1"
    }
    [%u|b%"tmp_name"]=>
    array(1) {
      [0]=>
      %unicode|string%(%d) "%s"
    }
    [%u|b%"error"]=>
    array(1) {
      [0]=>
      int(0)
    }
    [%u|b%"size"]=>
    array(1) {
      [0]=>
      int(1)
    }
  }
}
array(0) {
}
