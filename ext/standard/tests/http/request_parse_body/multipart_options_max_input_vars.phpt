--TEST--
request_parse_body() max_input_vars option
--INI--
max_input_vars=10
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------84000087610663814162942123332
-----------------------------84000087610663814162942123332
Content-Disposition: form-data; name="field1"

post field data
-----------------------------84000087610663814162942123332
Content-Disposition: form-data; name="field2"

post field data
-----------------------------84000087610663814162942123332--
--FILE--
<?php

try {
    [$_POST, $_FILES] = request_parse_body([
        'max_input_vars' => 1,
    ]);
} catch (Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

var_dump($_POST, $_FILES);

?>
--EXPECT--
RequestParseBodyException: Input variables exceeded 1. To increase the limit change max_input_vars in php.ini.
array(0) {
}
array(0) {
}
