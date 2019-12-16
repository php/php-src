--TEST--
bug#44712 (stream_context_set_params segfaults on invalid arguments)
--FILE--
<?php
$ctx = stream_context_get_default();
try {
    stream_context_set_params($ctx, array("options" => 1));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
Invalid stream/context parameter
