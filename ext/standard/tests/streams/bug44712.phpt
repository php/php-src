--TEST--
bug#44712 (stream_context_set_params segfaults on invalid arguments)
--FILE--
<?php
$ctx = stream_context_get_default();
stream_context_set_params($ctx, array("options" => 1));
?>
--EXPECTF--
Warning: stream_context_set_params(): Invalid stream/context parameter in %sbug44712.php on line %d
