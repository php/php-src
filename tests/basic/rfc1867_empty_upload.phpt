--TEST--
rfc1867 empty upload
--INI--
file_uploads=1
upload_max_filesize=1024
max_file_uploads=10
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="foo"


-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain-file1

1
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="file2"; filename=""
Content-Type: text/plain-file2


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
  array(6) {
    ["name"]=>
    string(9) "file1.txt"
    ["full_path"]=>
    string(9) "file1.txt"
    ["type"]=>
    string(16) "text/plain-file1"
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
    string(0) ""
    ["full_path"]=>
    string(0) ""
    ["type"]=>
    string(0) ""
    ["tmp_name"]=>
    string(0) ""
    ["error"]=>
    int(4)
    ["size"]=>
    int(0)
  }
  ["file3"]=>
  array(6) {
    ["name"]=>
    string(9) "file3.txt"
    ["full_path"]=>
    string(9) "file3.txt"
    ["type"]=>
    string(16) "text/plain-file3"
    ["tmp_name"]=>
    string(%d) "%s"
    ["error"]=>
    int(0)
    ["size"]=>
    int(1)
  }
}
array(1) {
  ["foo"]=>
  string(0) ""
}
string(1) "1"
string(1) "3"
