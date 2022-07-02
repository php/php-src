--TEST--
Request #77372 (Relative file path is removed from uploaded file)
--INI--
file_uploads=1
upload_max_filesize=1024
max_file_uploads=10
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------64369134225794159231042985467
-----------------------------64369134225794159231042985467
Content-Disposition: form-data; name="files[]"; filename="directory/subdirectory/file2.txt"
Content-Type: text/plain

2
-----------------------------64369134225794159231042985467
Content-Disposition: form-data; name="files[]"; filename="directory/file1.txt"
Content-Type: text/plain

1
-----------------------------64369134225794159231042985467--
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
?>
--EXPECTF--
array(1) {
  ["files"]=>
  array(6) {
    ["name"]=>
    array(2) {
      [0]=>
      string(9) "file2.txt"
      [1]=>
      string(9) "file1.txt"
    }
    ["full_path"]=>
    array(2) {
      [0]=>
      string(32) "directory/subdirectory/file2.txt"
      [1]=>
      string(19) "directory/file1.txt"
    }
    ["type"]=>
    array(2) {
      [0]=>
      string(10) "text/plain"
      [1]=>
      string(10) "text/plain"
    }
    ["tmp_name"]=>
    array(2) {
      [0]=>
      string(%d) "%s"
      [1]=>
      string(%d) "%s"
    }
    ["error"]=>
    array(2) {
      [0]=>
      int(0)
      [1]=>
      int(0)
    }
    ["size"]=>
    array(2) {
      [0]=>
      int(1)
      [1]=>
      int(1)
    }
  }
}
array(0) {
}
