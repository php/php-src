--TEST--
request_parse_body() invalid key
--FILE--
<?php

try {
    request_parse_body(options: ['foo' => 1]);
} catch (Error $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

try {
    request_parse_body(options: [42 => 1]);
} catch (Error $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Invalid key "foo" in $options argument
ValueError: Invalid integer key in $options argument
