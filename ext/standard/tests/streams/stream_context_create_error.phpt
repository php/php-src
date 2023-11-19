--TEST--
Test the error cases of stream_context_create()
--FILE--
<?php
try {
    stream_context_create(['ssl' => "abc"]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    stream_context_create(['ssl' => ['verify_peer'=> false]], ["options" => ['ssl' => "abc"]]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    stream_context_create(['ssl' => ['verify_peer'=> false]], ["options" => false]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
Options should have the form ["wrappername"]["optionname"] = $value
Options should have the form ["wrappername"]["optionname"] = $value
Invalid stream/context parameter
