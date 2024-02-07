--TEST--
request_parse_body() post_max_size option
--INI--
post_max_size=1M
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------84000087610663814162942123332
-----------------------------84000087610663814162942123332
Content-Disposition: form-data; name="field1"

post field data
-----------------------------84000087610663814162942123332
Content-Disposition: form-data; name="field2"

post file data
-----------------------------84000087610663814162942123332--
--FILE--
<?php

try {
    [$_POST, $_FILES] = request_parse_body([
        'post_max_size' => '302',
    ]);
} catch (Exception $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

var_dump($_POST, $_FILES);

?>
--EXPECT--
Exception: POST Content-Length of 303 bytes exceeds the limit of 302 bytes
array(0) {
}
array(0) {
}
