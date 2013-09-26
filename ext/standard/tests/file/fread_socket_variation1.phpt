--TEST--
Testing fread() on a TCP server socket
--FILE--
<?php

for ($i=0; $i<100; $i++) {
  $port = rand(10000, 65000);
  /* Setup socket server */
  $server = @stream_socket_server("tcp://127.0.0.1:$port");
  if ($server) {
    break;
  }
}

socket_set_timeout($server, 0, 1000);

var_dump(fread($server, 1));

fclose($server);

?>
--EXPECT--
string(0) ""
