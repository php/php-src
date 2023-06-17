--TEST--
request_parse_body() with multipart and garbled field
--INI--
max_file_uploads=1
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------84000087610663814162942123332
-----------------------------84000087610663814162942123332
Content-Disposition: form-data;
Content-Type: text/plain

post field data
-----------------------------84000087610663814162942123332--
--FILE--
<?php

try {
    [$_POST, $_FILES] = request_parse_body();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

var_dump($_POST, $_FILES);

?>
--EXPECT--
File Upload Mime headers garbled
array(0) {
}
array(0) {
}
