--TEST--
request_parse_body() with multipart and exceeding max files
--INI--
max_file_uploads=1
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------84000087610663814162942123332
-----------------------------84000087610663814162942123332
Content-Disposition: form-data; name="file1"; filename="file1.txt"
Content-Type: text/plain

file data
-----------------------------84000087610663814162942123332
Content-Disposition: form-data; name="file2"; filename="file2.txt"
Content-Type: text/plain

file data
-----------------------------84000087610663814162942123332--
--FILE--
<?php

try {
    [$_POST, $_FILES] = request_parse_body();
} catch (Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

var_dump($_POST, $_FILES);

?>
--EXPECT--
RequestParseBodyException: Maximum number of allowable file uploads has been exceeded
array(0) {
}
array(0) {
}
