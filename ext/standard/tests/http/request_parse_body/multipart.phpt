--TEST--
request_parse_body() with multipart
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------84000087610663814162942123332
-----------------------------84000087610663814162942123332
Content-Disposition: form-data; name="post_field_name"

post field data
-----------------------------84000087610663814162942123332
Content-Disposition: form-data; name="file_name"; filename="original_file_name.txt"
Content-Type: text/plain

file data
-----------------------------84000087610663814162942123332--
--FILE--
<?php

[$_POST, $_FILES] = request_parse_body();

var_dump($_POST, $_FILES);

$file_path = __DIR__ . '/put_multipart_uploaded_file.txt';
move_uploaded_file($_FILES['file_name']['tmp_name'], $file_path);
var_dump(file_get_contents($file_path));

?>
--CLEAN--
<?php
$file_path = __DIR__ . '/put_multipart_uploaded_file.txt';
@unlink($file_path);
?>
--EXPECTF--
array(1) {
  ["post_field_name"]=>
  string(15) "post field data"
}
array(1) {
  ["file_name"]=>
  array(6) {
    ["name"]=>
    string(22) "original_file_name.txt"
    ["full_path"]=>
    string(22) "original_file_name.txt"
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
string(9) "file data"
