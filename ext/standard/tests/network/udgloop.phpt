--TEST--
Streams Based Unix Domain Datagram Loopback test
--SKIPIF--
<?php
    if (array_search("udg",stream_get_transports()) === false)
        die('SKIP No support for UNIX domain sockets.');
?>
--FILE--
<?php
    $uniqid = uniqid();
    if (file_exists("/tmp/$uniqid.sock"))
        die('Temporary socket /tmp/$uniqid.sock already exists.');

    /* Setup socket server */
    $server = stream_socket_server("udg:///tmp/$uniqid.sock", $errno, $errstr, STREAM_SERVER_BIND);
    if (!$server) {
        die('Unable to create AF_UNIX socket [server]');
    }

    /* Connect to it */
    $client = stream_socket_client("udg:///tmp/$uniqid.sock");
    if (!$client) {
        die('Unable to create AF_UNIX socket [client]');
    }

    fwrite($client, "ABCdef123\n");

    $data = fread($server, 10);
    var_dump($data);

    fclose($client);
    fclose($server);
    unlink("/tmp/$uniqid.sock");
?>
--EXPECT--
string(10) "ABCdef123
"
