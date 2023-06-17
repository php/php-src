--TEST--
request_parse_body() invalid key
--FILE--
<?php

try {
    request_parse_body(options: ['foo' => 1]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    request_parse_body(options: ['moo' => 1]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Invalid key "foo" in $options argument
Invalid key "moo" in $options argument
