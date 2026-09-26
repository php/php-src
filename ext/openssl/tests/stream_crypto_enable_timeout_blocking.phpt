--TEST--
OpenSSL handshake timeout restores the stream blocking mode
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip no proc_open');
?>
--FILE--
<?php
$serverCode = <<<'CODE'
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server, 5);
    phpt_wait();
    fclose($conn);
CODE;

$clientCode = <<<'CODE'
    $context = stream_context_create(['ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
    ]]);
    $client = stream_socket_client('tcp://{{ ADDR }}', $errno, $errstr, 1, STREAM_CLIENT_CONNECT, $context);

    $metadata = stream_get_meta_data($client);
    var_dump($metadata['blocked']);

    set_error_handler(function (int $errno, string $errstr) use ($client): bool {
        $metadata = stream_get_meta_data($client);
        var_dump($metadata['blocked']);
        echo $errstr, "\n";
        fclose($client);
        return true;
    });

    var_dump(stream_socket_enable_crypto($client, true, STREAM_CRYPTO_METHOD_TLS_CLIENT));
    restore_error_handler();
    var_dump(is_resource($client));

    phpt_notify();
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
bool(true)
bool(true)
stream_socket_enable_crypto(): SSL: Handshake timed out
bool(false)
bool(false)
