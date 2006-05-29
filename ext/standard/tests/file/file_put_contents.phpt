--TEST--
file_put_contents() and invalid parameters
--FILE--
<?php

$file = dirname(__FILE__)."/file_put_contents.txt";

$context = stream_context_create();

var_dump(file_put_contents($file, $context));
var_dump(file_put_contents($file, new stdClass));
$fp = fopen($file, "r");
var_dump(file_put_contents($file, "string", 0, $fp));

@unlink($file);

echo "Done\n";
?>
--EXPECTF--	
Warning: file_put_contents(): supplied resource is not a valid stream resource in %s on line %d
bool(false)

Warning: file_put_contents(): The 2nd parameter should be either a string or an array in %s on line %d
bool(false)

Warning: file_put_contents(): supplied resource is not a valid Stream-Context resource in %s on line %d
int(6)
Done
