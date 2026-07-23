--TEST--
stream_filter_register() with a class name that does not exist
--FILE--
<?php

try {
	var_dump(stream_filter_register("not_existing_filter", "not_existing"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

stream_filter_append(STDOUT, "not_existing_filter");

$out = fwrite(STDOUT, "Hello\n");
var_dump($out);

?>
--EXPECTF--
TypeError: stream_filter_register(): Argument #2 ($class) must be a valid class name, not_existing given

Warning: stream_filter_append(): Unable to locate filter "not_existing_filter" in %s on line %d
Hello
int(6)
