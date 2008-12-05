--TEST--
Test stream_set_timeout() function : error conditions 
--FILE--
<?php
/* Prototype  : proto bool stream_set_timeout(resource stream, int seconds, int microseconds)
 * Description: Set timeout on stream read to seconds + microseonds 
 * Source code: ext/standard/streamsfuncs.c
 * Alias to functions: socket_set_timeout
 */

echo "*** Testing stream_set_timeout() : error conditions ***\n";

//Test stream_set_timeout with one more than the expected number of arguments
echo "\n-- Testing stream_set_timeout() function with more than expected no. of arguments --\n";

/* Setup socket server */
$server = stream_socket_server('tcp://127.0.0.1:31337');
/* Connect to it */
$client = fsockopen('tcp://127.0.0.1:31337');

$seconds = 10;
$microseconds = 10;
$extra_arg = 10;
var_dump( stream_set_timeout($client, $seconds, $microseconds, $extra_arg) );

// Testing stream_set_timeout with one less than the expected number of arguments
echo "\n-- Testing stream_set_timeout() function with less than expected no. of arguments --\n";

$seconds = 10;
var_dump( stream_set_timeout($client) );


echo "\n-- Testing stream_set_timeout() function with a closed socket --\n";
fclose($client);
var_dump( stream_set_timeout($client, $seconds) );

echo "\n-- Testing stream_set_timeout() function with an invalid stream --\n";
var_dump( stream_set_timeout($seconds, $seconds) );

echo "\n-- Testing stream_set_timeout() function with a stream that does not support timeouts --\n";
$filestream = fopen(__FILE__, "r");
var_dump( stream_set_timeout($filestream, $seconds) );

fclose($filestream);
fclose($server);

echo "Done";
?>
--EXPECTF--
*** Testing stream_set_timeout() : error conditions ***

-- Testing stream_set_timeout() function with more than expected no. of arguments --

Warning: stream_set_timeout() expects at most 3 parameters, 4 given in %s on line %i
NULL

-- Testing stream_set_timeout() function with less than expected no. of arguments --

Warning: stream_set_timeout() expects at least 2 parameters, 1 given in %s on line %i
NULL

-- Testing stream_set_timeout() function with a closed socket --

Warning: stream_set_timeout(): %i is not a valid stream resource in %s on line %i
bool(false)

-- Testing stream_set_timeout() function with an invalid stream --

Warning: stream_set_timeout() expects parameter 1 to be resource, integer given in %s on line %i
NULL

-- Testing stream_set_timeout() function with a stream that does not support timeouts --
bool(false)
Done
