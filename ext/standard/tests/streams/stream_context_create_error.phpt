--TEST--
Test the error cases of stream_context_create()
--FILE--
<?php
$arr = [];
try {
    stream_context_create($arr);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    stream_context_create(['ssl' => ['verify_peer'=> false]], ["options" => $arr]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    stream_context_create(['ssl' => ['verify_peer'=> false]], ["options" => false]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
Options should have the form ["wrappername"]["optionname"] = $value
Options should have the form ["wrappername"]["optionname"] = $value
Invalid stream/context parameter
