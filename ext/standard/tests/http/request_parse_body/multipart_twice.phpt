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

request_parse_body();

var_dump(is_uploaded_file($tmp_name));

?>
--EXPECT--
bool(true)
bool(true)
