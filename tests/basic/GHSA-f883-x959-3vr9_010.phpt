--TEST--
GHSA-f883-x959-3vr9 010
--POST_RAW--
Content-Type: multipart/form-data; boundary=b
--b
Content-Disposition: form-data; name="f1"; filename="f1"
Content-Disposition: form-data; name="f2"; filename="f2"
Content-Type: text/plain

test
--b
Content-Disposition: form-data; name="d1"
Content-Disposition: form-data; name="d2"

test
--b--
--FILE--
<?php

var_dump($_POST);
var_dump($_FILES);

?>
--EXPECTF--
array(1) {
  ["d1"]=>
  string(4) "test"
}
array(1) {
  ["f1"]=>
  array(6) {
    ["name"]=>
    string(2) "f1"
    ["full_path"]=>
    string(2) "f1"
    ["type"]=>
    string(10) "text/plain"
    ["tmp_name"]=>
    string(%d) "%s"
    ["error"]=>
    int(0)
    ["size"]=>
    int(4)
  }
}
