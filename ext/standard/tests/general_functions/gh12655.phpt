--TEST--
GH-12655 (proc_open(): Argument #2 ($descriptor_spec) must only contain arrays and streams [Descriptor item must be either an array or a File-Handle])
--FILE--
<?php

$descriptor_spec = [
    0 => [ "pipe", "r" ],  // stdin is a pipe that the child will read from
    1 => [ "pipe", "w" ],  // stdout is a pipe that the child will write to
    2 => [ "pipe", "w" ],  // stderr is a file to write to
];

foreach ( $descriptor_spec as $fd => &$d )
{
    // don't do anything, just the fact that we used "&$d" will sink the ship!
}

$proc = proc_open(PHP_BINARY, $descriptor_spec, $pipes);
echo $proc === false ? "FAILED\n" : "SUCCEEDED\n";

?>
--EXPECT--
SUCCEEDED
