--TEST--
Test stream_bucket_new function with invalid stream.
--CREDITS--
Vinicius Dias carlosv775@gmail.com
--FILE--
<?php
$bucket = stream_bucket_new(null, 'test');
var_dump($bucket);
?>
--EXPECTF--
Warning: stream_bucket_new(): supplied argument is not a valid stream resource in %s on line %d
bool(false)
