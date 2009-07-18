--TEST--
rfc1867 anonymous upload
--INI--
file_uploads=1
error_reporting=E_ALL&~E_NOTICE
comment=debug builds show some additional E_NOTICE errors
upload_max_filesize=1024
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; filename="file1.txt"
Content-Type: text/plain-file1

1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; filename="file2.txt"
Content-Type: text/plain-file2

2
-----------------------------20896060251896012921717172737--
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
?>
--EXPECTF--
array(2) {
  [%d]=>
  array(5) {
    [%u|b%"name"]=>
    %string|unicode%(9) "file1.txt"
    [%u|b%"type"]=>
    %string|unicode%(16) "text/plain-file1"
    [%u|b%"tmp_name"]=>
    %string|unicode%(%d) "%s"
    [%u|b%"error"]=>
    int(0)
    [%u|b%"size"]=>
    int(1)
  }
  [%d]=>
  array(5) {
    [%u|b%"name"]=>
    %string|unicode%(9) "file2.txt"
    [%u|b%"type"]=>
    %string|unicode%(16) "text/plain-file2"
    [%u|b%"tmp_name"]=>
    %string|unicode%(%d) "%s"
    [%u|b%"error"]=>
    int(0)
    [%u|b%"size"]=>
    int(1)
  }
}
array(0) {
}
