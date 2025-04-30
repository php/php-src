--TEST--
request_parse_body() with multipart and invalid boundary
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: multipart/form-data; boundary="foobar
empty
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
RequestParseBodyException: Invalid boundary in multipart/form-data POST data
array(0) {
}
array(0) {
}
