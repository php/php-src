--TEST--
Bug #51056 (fread() on blocking stream will block even if data is available)
--FILE--
<?php

$serverCode = <<<'CODE'
$server = stream_socket_server('tcp://127.0.0.1:64324');
phpt_notify();

$conn = stream_socket_accept($server);

fwrite($conn, "Hello 1\n"); // 8 bytes
usleep(50000);
fwrite($conn, str_repeat('a', 300)."\n"); // 301 bytes
usleep(50000);
fwrite($conn, "Hello 1\n"); // 8 bytes

fclose($conn);
fclose($server);
CODE;

$clientCode = <<<'CODE'

phpt_wait();

$fp = fsockopen("tcp://127.0.0.1:64324");

while (!feof($fp)) {
    $data = fread($fp, 256);
    $bytes = strlen($data);
    if ($bytes > 0) {
        printf("fread read %d bytes\n", $bytes);
    }
}
CODE;

include sprintf("%s/../../../openssl/tests/ServerClientTestCase.inc", __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);

?>
--EXPECT--
fread read 8 bytes
fread read 256 bytes
fread read 45 bytes
fread read 8 bytes
