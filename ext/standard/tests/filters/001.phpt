--TEST--
stream_filter_register() and invalid arguments
--FILE--
<?php
try {
    stream_filter_register("", "stdClass");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    stream_filter_register("test", "Throwable");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	var_dump(stream_filter_register("------", "nonexistentclass"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
ValueError: stream_filter_register(): Argument #1 ($filter_name) must be a non-empty string
ValueError: stream_filter_register(): Argument #2 ($class) must be a concrete class
TypeError: stream_filter_register(): Argument #2 ($class) must be a valid class name, nonexistentclass given
Done
