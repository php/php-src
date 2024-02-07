--TEST--
request_parse_body() max_multipart_body_parts option
--INI--
max_multipart_body_parts=10
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

try {
    [$_POST, $_FILES] = request_parse_body([
        'max_multipart_body_parts' => 1,
    ]);
} catch (Exception $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

var_dump($_POST, $_FILES);

?>
--EXPECT--
Exception: Multipart body parts limit exceeded 1. To increase the limit change max_multipart_body_parts in php.ini.
array(0) {
}
array(0) {
}
