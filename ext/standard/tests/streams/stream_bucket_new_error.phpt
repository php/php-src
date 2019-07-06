--TEST--
Test stream_bucket_new function with invalid stream.
--DESCRIPTION--
If we define an invalid stream to the stream_bucket_new function it should raise a warning and return false.
Looks like in PHP 8 this is going to throw a Fatal Error
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