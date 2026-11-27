--TEST--
stream_context_set_params() and stream_context_create() with invalid notification option
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
try {
    var_dump(stream_context_create(null, ["notification" => "fn_not_exist"]));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: stream_context_set_params(): Argument #2 ($params) must have a valid callback for key "notification", function "fn_not_exist" not found or invalid function name
TypeError: stream_context_set_params(): Argument #2 ($params) must have a valid callback for key "notification", class "myclass" not found
TypeError: stream_context_create(): Argument #2 ($params) must have a valid callback for key "notification", function "fn_not_exist" not found or invalid function name
