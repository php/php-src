--TEST--
stream_context_set_option() error conditions
--FILE--
<?php

$ctx = stream_context_create();
try {
    stream_context_set_option($ctx, [], "x");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    stream_context_set_option($ctx, [], null, "x");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    stream_context_set_option($ctx, "x");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    stream_context_set_option($ctx, "x", "y");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
stream_context_set_option(): Argument #3 ($option_name) must be null when argument #2 ($wrapper_or_options) is an array
stream_context_set_option(): Argument #4 ($value) cannot be provided when argument #2 ($wrapper_or_options) is an array
stream_context_set_option(): Argument #3 ($option_name) cannot be null when argument #2 ($wrapper_or_options) is a string
stream_context_set_option(): Argument #4 ($value) must be provided when argument #2 ($wrapper_or_options) is a string
