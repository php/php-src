--TEST--
rfc1867 missing boundary 2
--INI--
file_uploads=1
error_reporting=E_ALL&~E_NOTICE
comment=debug builds show some additional E_NOTICE errors
upload_max_filesize=1024
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain-file1

1
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
?>
--EXPECTF--
array(1) {
  [%u|b%"file1"]=>
  array(5) {
    [%u|b%"name"]=>
    %string|unicode%(9) "file1.txt"
    [%u|b%"type"]=>
    %string|unicode%(0) ""
    [%u|b%"tmp_name"]=>
    %string|unicode%(0) ""
    [%u|b%"error"]=>
    int(3)
    [%u|b%"size"]=>
    int(0)
  }
}
array(0) {
}
