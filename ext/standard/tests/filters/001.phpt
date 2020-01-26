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
--EXPECTF--
Filter name cannot be empty
Class name cannot be empty
Filter name cannot be empty
bool(true)
Done
