--TEST--
request_parse_body() with urlencoded
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: application/x-www-form-urlencoded
foo=foo&bar[]=1&bar[]=2
--FILE--
<?php

[$_POST, $_FILES] = request_parse_body();

var_dump($_POST, $_FILES);

?>
--CLEAN--
<?php
$file_path = __DIR__ . '/put_multipart_uploaded_file.txt';
@unlink($file_path);
?>
--EXPECT--
array(2) {
  ["foo"]=>
  string(3) "foo"
  ["bar"]=>
  array(2) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
  }
}
array(0) {
}
