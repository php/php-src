--TEST--
request_parse_body() called twice keeps files uploaded by the first call
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------84000087610663814162942123332
-----------------------------84000087610663814162942123332
Content-Disposition: form-data; name="file_name"; filename="original_file_name.txt"
Content-Type: text/plain

file data
-----------------------------84000087610663814162942123332--
--FILE--
<?php

[, $files] = request_parse_body();
$tmp_name = $files['file_name']['tmp_name'];
var_dump(is_uploaded_file($tmp_name));

var_dump(request_parse_body());

var_dump(is_uploaded_file($tmp_name));

$dst = __DIR__ . '/multipart_twice.txt';
var_dump(move_uploaded_file($tmp_name, $dst));
var_dump(file_get_contents($dst));

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/multipart_twice.txt');
?>
--EXPECT--
bool(true)
array(2) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(0) {
  }
}
bool(true)
bool(true)
string(9) "file data"
