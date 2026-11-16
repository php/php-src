--TEST--
GHSA-f883-x959-3vr9 002
--POST_RAW--
Content-Type: multipart/form-data; boundary=b
--b
Ignored: value
Content-Disposition: form-data; name="d1"

test
--b--
--FILE--
<?php

var_dump($_POST);

?>
--EXPECT--
array(1) {
  ["d1"]=>
  string(4) "test"
}
