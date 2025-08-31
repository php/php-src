--TEST--
request_parse_body() with multipart and unsupported Content-Type
--INI--
max_input_vars=1
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: application/json
{"hello": "world"}
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
RequestParseBodyException: Content-Type "application/json" is not supported
array(0) {
}
array(0) {
}
