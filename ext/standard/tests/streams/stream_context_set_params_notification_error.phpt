--TEST--
stream_context_set_params() with invalid notification option
--FILE--
<?php

$ctx = stream_context_create();
try {
    var_dump(stream_context_set_params($ctx, ["notification" => "fn_not_exist"]));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(stream_context_set_params($ctx, ["notification" => ["myclass", "fn_not_exist"]]));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: stream_context_set_params(): Argument #1 ($context) must be an array with valid callbacks as values, function "fn_not_exist" not found or invalid function name
TypeError: stream_context_set_params(): Argument #1 ($context) must be an array with valid callbacks as values, class "myclass" not found
