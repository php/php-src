--TEST--
rfc1867 post_max_filesize
--INI--
file_uploads=1
upload_max_filesize=1
max_file_uploads=10
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain-file1

1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file2"; filename="file2.txt"
Content-Type: text/plain-file2

22
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file3"; filename="file3.txt"
Content-Type: text/plain-file3

3
-----------------------------20896060251896012921717172737--
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
if (is_uploaded_file($_FILES["file1"]["tmp_name"])) {
	var_dump(file_get_contents($_FILES["file1"]["tmp_name"]));
}
if (is_uploaded_file($_FILES["file3"]["tmp_name"])) {
	var_dump(file_get_contents($_FILES["file3"]["tmp_name"]));
}
?>
--EXPECTF--
array(3) {
  ["file1"]=>
  array(5) {
    ["name"]=>
    %string|unicode%(9) "file1.txt"
    ["type"]=>
    %string|unicode%(16) "text/plain-file1"
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
    %string|unicode%(0) ""
    ["error"]=>
    int(1)
    ["size"]=>
    int(0)
  }
  ["file3"]=>
  array(5) {
    ["name"]=>
    %string|unicode%(9) "file3.txt"
    ["type"]=>
    %string|unicode%(16) "text/plain-file3"
    ["tmp_name"]=>
    %string|unicode%(%d) "%s"
    ["error"]=>
    int(0)
    ["size"]=>
    int(1)
  }
}
array(0) {
}
string(1) "1"
string(1) "3"
