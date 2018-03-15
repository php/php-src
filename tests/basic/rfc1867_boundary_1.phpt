--TEST--
rfc1867 boundary 1
--INI--
post_max_size=1024
--POST_RAW--
Content-Type: multipart/form-data; boundary="------------------------------------foobar"
--------------------------------------foobar
Content-Disposition: form-data; name="foobar"

1
--------------------------------------foobar--
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
?>
--EXPECT--
array(0) {
}
array(1) {
  ["foobar"]=>
  string(1) "1"
}
