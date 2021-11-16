--TEST--
ext/sockets - socket_bind - basic test
--CREDITS--
Florian Anderiasch
fa@php.net
--EXTENSIONS--
sockets
--SKIPIF--
<?php
    
    if (getenv("SKIP_ONLINE_TESTS")) {
        die("skip test requiring internet connection");
    }
?>
--FILE--
<?php
    $s_c     = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    $s_bind  = socket_bind($s_c, '0.0.0.0');
    var_dump($s_bind);

    // Connect to destination address
    $s_conn  = socket_connect($s_c, 'www.php.net', 80);
    var_dump($s_conn);

    // Write
    $request = 'GET / HTTP/1.1' . "\r\n";
    $s_write = socket_write($s_c, $request);
    var_dump($s_write);

    // Close
    $s_close = socket_close($s_c);
    var_dump($s_close);
?>
--EXPECT--
bool(true)
bool(true)
int(16)
NULL
