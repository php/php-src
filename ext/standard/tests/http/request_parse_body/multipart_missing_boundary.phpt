--TEST--
request_parse_body() with multipart and missing boundary
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: multipart/form-data
empty
--FILE--
<?php

$stream = fopen('php://memory','r+');

try {
    [$_POST, $_FILES] = request_parse_body();
} catch (Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

var_dump($_POST, $_FILES);

?>
--EXPECT--
RequestParseBodyException: Missing boundary in multipart/form-data POST data
array(0) {
}
array(0) {
}
