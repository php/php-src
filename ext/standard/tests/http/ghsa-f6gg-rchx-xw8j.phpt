--TEST--
GHSA-f6gg-rchx-xw8j: mixed-case HTTPS URLs must use TLS
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists('proc_open')) die('skip proc_open() not available');
?>
--FILE--
<?php
$serverCode = <<<'CODE'
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
    phpt_notify_server_start($server);

    $conn = stream_socket_accept($server);
    $request = fread($conn, 1);

    if ($request === "\x16") {
        $result = 'TLS';
    } else {
        while (($line = stream_get_line($conn, 8192, "\r\n")) !== false) {
            $request .= $line . "\r\n";
            if ($line === '') {
                break;
            }
        }
        $request .= stream_get_contents($conn, strlen('opensec-sensitive-body'));
        $result = str_contains($request, 'Authorization: Basic ')
            && str_contains($request, 'X-Opensec-Request: validation')
            && str_contains($request, 'opensec-sensitive-body')
                ? 'plaintext credentials and body'
                : 'unexpected plaintext request';
    }

    fclose($conn);
    fclose($server);
    phpt_notify(message: $result);
CODE;

$clientCode = <<<'CODE'
    $context = stream_context_create([
        'http' => [
            'method' => 'POST',
            'content' => 'opensec-sensitive-body',
            'header' => "X-Opensec-Request: validation\r\n",
        ],
        'ssl' => [
            'verify_peer' => false,
            'verify_peer_name' => false,
        ],
    ]);
    var_dump(@file_get_contents('HTTPS://alice:secret@{{ ADDR }}/secret', false, $context));
    echo phpt_wait();
CODE;

include sprintf('%s/../../../openssl/tests/ServerClientTestCase.inc', __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
bool(false)
TLS
