--TEST--
Bug #37276 (problems witch $_POST array)
--INI--
file_upload=1
--SKIPIF--
<?php if (php_sapi_name()=='cli') die('skip'); ?>
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="submitter"

testname
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="pics"; filename="bug37276.txt"
Content-Type: text/plain

bug37276

-----------------------------20896060251896012921717172737--
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
?>
--EXPECTF--
array(1) {
  ["pics"]=>
  array(5) {
    ["name"]=>
    string(12) "bug37276.txt"
    ["type"]=>
    string(10) "text/plain"
    ["tmp_name"]=>
    string(%d) "%s"
    ["error"]=>
    int(0)
    ["size"]=>
    int(9)
  }
}
array(1) {
  ["submitter"]=>
  string(8) "testname"
}
