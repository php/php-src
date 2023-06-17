--TEST--
request_parse_body() invalid value type
--FILE--
<?php

try {
    request_parse_body(options: [
        'max_input_vars' => [],
    ]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Invalid array value in $options argument
