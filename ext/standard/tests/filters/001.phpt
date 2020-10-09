--TEST--
stream_filter_register() and invalid arguments
--FILE--
<?php
try {
    stream_filter_register("", "");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    stream_filter_register("test", "");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    stream_filter_register("", "test");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(stream_filter_register("------", "nonexistentclass"));

echo "Done\n";
?>
--EXPECT--
stream_filter_register(): Argument #1 ($filter_name) must be a non-empty string
stream_filter_register(): Argument #2 ($class) must be a non-empty string
stream_filter_register(): Argument #1 ($filter_name) must be a non-empty string
bool(true)
Done
