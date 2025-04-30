--TEST--
request_parse_body() invalid value type
--FILE--
<?php

try {
    request_parse_body(options: [
        'max_input_vars' => [],
    ]);
} catch (Error $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Invalid array value in $options argument
