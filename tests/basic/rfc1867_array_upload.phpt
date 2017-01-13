--TEST--
rfc1867 array upload
--INI--
file_uploads=1
upload_max_filesize=1024
max_file_uploads=10
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file[]"; filename="file1.txt"
Content-Type: text/plain-file1

1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file[2]"; filename="file2.txt"
Content-Type: text/plain-file2

2
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file[]"; filename="file3.txt"
Content-Type: text/plain-file3

3
-----------------------------20896060251896012921717172737--
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
?>
--EXPECTF--
array(1) {
  ["file"]=>
  array(5) {
    ["name"]=>
    array(3) {
      [0]=>
      string(9) "file1.txt"
      [2]=>
      string(9) "file2.txt"
      [3]=>
      string(9) "file3.txt"
    }
    ["type"]=>
    array(3) {
      [0]=>
      string(16) "text/plain-file1"
      [2]=>
      string(16) "text/plain-file2"
      [3]=>
      string(16) "text/plain-file3"
    }
    ["tmp_name"]=>
    array(3) {
      [0]=>
      string(%d) "%s"
      [2]=>
      string(%d) "%s"
      [3]=>
      string(%d) "%s"
    }
    ["error"]=>
    array(3) {
      [0]=>
      int(0)
      [2]=>
      int(0)
      [3]=>
      int(0)
    }
    ["size"]=>
    array(3) {
      [0]=>
      int(1)
      [2]=>
      int(1)
      [3]=>
      int(1)
    }
  }
}
array(0) {
}
