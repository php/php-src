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

?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__)."/file_put_contents.txt"); ?>
--EXPECTF--
Warning: file_put_contents(): supplied resource is not a valid stream resource in %s on line %d
bool(false)

Warning: file_put_contents(): 2nd parameter must be non-object (for now) in %s on line %d
bool(false)

Warning: file_put_contents(): supplied resource is not a valid Stream-Context resource in %s on line %d

Notice: file_put_contents(): 6 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
int(6)
===DONE===
