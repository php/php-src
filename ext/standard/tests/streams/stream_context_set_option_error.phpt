--TEST--
stream_context_set_option() error conditions
--FILE--
<?php

$ctx = stream_context_create();
try {
    stream_context_set_option($ctx, [], "x");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    stream_context_set_option($ctx, [], null, "x");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    stream_context_set_option($ctx, "x");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    stream_context_set_option($ctx, "x", "y");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
ValueError: stream_context_set_option(): Argument #3 ($option_name) must be null when argument #2 ($wrapper_or_options) is an array
ValueError: stream_context_set_option(): Argument #4 ($value) cannot be provided when argument #2 ($wrapper_or_options) is an array

Deprecated: Calling stream_context_set_option() with 2 arguments is deprecated, use stream_context_set_options() instead in %s on line %d
ValueError: stream_context_set_option(): Argument #3 ($option_name) cannot be null when argument #2 ($wrapper_or_options) is a string
ValueError: stream_context_set_option(): Argument #4 ($value) must be provided when argument #2 ($wrapper_or_options) is a string
