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
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

var_dump($_POST, $_FILES);

?>
--EXPECT--
Missing boundary in multipart/form-data POST data
array(0) {
}
array(0) {
}
