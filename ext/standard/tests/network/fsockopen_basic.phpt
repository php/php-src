--TEST--
Test fsockopen() function : basic functionality
--FILE--
<?php
/* Prototype  : proto resource fsockopen(string hostname, int port [, int errno [, string errstr [, float timeout]]])
 * Description: Open Internet or Unix domain socket connection
 * Source code: ext/standard/fsock.c
 * Alias to functions:
 */

echo "*** Testing fsockopen() : basic functionality ***\n";

echo "Open a server socket\n";

for ($i=0; $i<100; $i++) {
  $port = rand(10000, 65000);
  /* Setup socket server */
  $server = @stream_socket_server("tcp://127.0.0.1:$port");
  if ($server) {
    break;
  }
}

// Initialise all required variables
$hostname = 'tcp://127.0.0.1'; // loopback address
$errno = null;
$errstr = null;
$timeout = 1.5;

echo "\nCalling fsockopen() with all possible arguments:\n";
$client = fsockopen($hostname, $port, $errno, $errstr, $timeout);
var_dump($client);
fclose($client);

echo "\nCalling fsockopen() with mandatory arguments:\n";
$second_client = fsockopen($hostname, $port);
var_dump($second_client);
fclose($second_client);

echo "\nCalling fsockopen() with address and port in same string:\n";
$address = $hostname . ':' . $port;
$third_client = fsockopen($address);
var_dump($third_client);
fclose($third_client);

echo "Done";
?>
--EXPECTF--
*** Testing fsockopen() : basic functionality ***
Open a server socket

Calling fsockopen() with all possible arguments:
resource(%d) of type (stream)

Calling fsockopen() with mandatory arguments:
resource(%d) of type (stream)

Calling fsockopen() with address and port in same string:
resource(%d) of type (stream)
Done
