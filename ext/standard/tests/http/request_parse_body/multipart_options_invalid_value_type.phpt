--TEST--
request_parse_body() invalid value type
--FILE--
<?php

try {
    request_parse_body(options: [
        'max_input_vars' => [],
    ]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Invalid array value in $options argument
