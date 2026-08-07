--TEST--
Test the error cases of stream_context_create()
--FILE--
<?php
try {
    stream_context_create(['ssl' => "abc"]);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    stream_context_create(['ssl' => ['verify_peer'=> false]], ["options" => ['ssl' => "abc"]]);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    stream_context_create(['ssl' => ['verify_peer'=> false]], ["options" => false]);
} catch (TypeError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: Options should have the form ["wrappername"]["optionname"] = $value
ValueError: Options should have the form ["wrappername"]["optionname"] = $value
TypeError: Invalid stream/context parameter
