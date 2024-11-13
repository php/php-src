--TEST--
Error test for stream_context_set_options()
--FILE--
<?php
$pipes = [];
$description = array(
   0 => array("pipe", "r"),
   1 => array("pipe", "w"),
   2 => array("pipe", "r")
);

$process = proc_open('nothing', $description, $pipes);

try {
    stream_context_set_options($process, []);
} catch (TypeError $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
stream_context_set_options(): Argument #1 ($context) must be a valid stream/context
