--TEST--
request_parse_body() invalid value type
--FILE--
<?php

try {
    request_parse_body(options: [
        'max_input_vars' => [],
    ]);
} catch (Error $e) {
    echo get_class($) $e->getMessage(), "\n";
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token ")", expecting variable or "{" or "$" in %s on line %d
