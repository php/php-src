--TEST--
Bug #74429 Remote socket URI with unique persistence identifier broken
--FILE--
<?php

/* This behavior is undocumented, but might be in use. Until there's no officially
    supported alternative, ensure changes doesn't cause BC breach. Otherwise,
    the test should be removed once the undocumented behavior changes. */

for ($i=0; $i<100; $i++) {
  $port = rand(10000, 65000);
  /* Setup socket server */
  $server = @stream_socket_server("tcp://127.0.0.1:$port");
  if ($server) {
    break;
  }
}

$client0 = stream_socket_client("tcp://127.0.0.1:$port/client0");
$client1 = stream_socket_client("tcp://127.0.0.1:$port/client1");

var_dump($client0, $client1);

fclose($server);
fclose($client0);
fclose($client1);

?>
--EXPECTF--
resource(%d) of type (stream)
resource(%d) of type (stream)
