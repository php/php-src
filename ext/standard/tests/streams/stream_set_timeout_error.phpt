--TEST--
Test stream_set_timeout() function : error conditions
--FILE--
<?php
echo "*** Testing stream_set_timeout() : error conditions ***\n";

for ($i=0; $i<100; $i++) {
  $port = rand(10000, 65000);
  /* Setup socket server */
  $server = @stream_socket_server("tcp://127.0.0.1:$port");
  if ($server) {
    break;
  }
}
/* Connect to it */
$client = fsockopen("tcp://127.0.0.1:$port");

$seconds = 10;
$microseconds = 10;

echo "\n-- Testing stream_set_timeout() function with a closed socket --\n";
fclose($client);
try {
    var_dump( stream_set_timeout($client, $seconds) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "\n-- Testing stream_set_timeout() function with a stream that does not support timeouts --\n";
$filestream = fopen(__FILE__, "r");
var_dump( stream_set_timeout($filestream, $seconds) );

fclose($filestream);
fclose($server);

echo "Done";
?>
--EXPECT--
*** Testing stream_set_timeout() : error conditions ***

-- Testing stream_set_timeout() function with a closed socket --
stream_set_timeout(): supplied resource is not a valid stream resource

-- Testing stream_set_timeout() function with a stream that does not support timeouts --
bool(false)
Done
