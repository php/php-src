--TEST--
Compressed protocol: packets below the threshold are sent uncompressed
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
ob_start();
if (extension_loaded('mysqlnd')) {
    (new ReflectionExtension('mysqlnd'))->info();
}
if (!str_contains(ob_get_clean(), 'Compression => supported')) {
    die('skip mysqlnd is built without compression support');
}
?>
--FILE--
<?php
require_once 'fake_server.inc';

$process = run_fake_server_in_background('compressed_queries');
$process->wait();

$link = mysqli_init();
$link->real_connect('127.0.0.1', 'root', '', '', $process->getPort(), null, MYSQLI_CLIENT_COMPRESS);
var_dump($link->query("SELECT '" . str_repeat('a', 80) . "'"));
var_dump($link->query("SELECT '" . str_repeat('a', 1000) . "'"));
$link->close();

$process->terminate(true);
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
[*] Connection established
[*] Sending - Server Greeting: 580000000a352e352e352d31302e352e31382d4d6172696144420003000000473e3f6047257c6700fef7080200ff81150000000000000f0000006c6b55463f49335f686c6431006d7973716c5f6e61746976655f70617373776f7264
[*] Received: %s
[*] Sending - Server OK: 0700000200000002000000
[*] Received compressed packet: 94 bytes, uncompressed length 0
[*] Sending - Compressed Server OK: 0b0000010000000700000100000002000000
bool(true)
[*] Received compressed packet: %d bytes, uncompressed length 1014
[*] Sending - Compressed Server OK: 0b0000010000000700000100000002000000
bool(true)
[*] Received compressed packet: 5 bytes, uncompressed length 0
[*] Server finished
