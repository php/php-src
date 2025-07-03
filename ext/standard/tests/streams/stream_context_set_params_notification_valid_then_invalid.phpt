--TEST--
stream_context_set_params() with valid, then invalid notification option
--FILE--
<?php

function foo() {}

$ctx = stream_context_create();
var_dump(stream_context_set_params($ctx, ["notification" => "foo"]));

try {
    var_dump(stream_context_set_params($ctx, ["notification" => "fn_not_exist"]));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(true)
TypeError: stream_context_set_params(): Argument #1 ($context) must be an array with valid callbacks as values, function "fn_not_exist" not found or invalid function name
