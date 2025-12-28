--TEST--
Bug #74796: TLS encryption fails behind HTTP proxy
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die("skip not reliable on Windows due to proxy wait limitation");
}
?>
--FILE--
<?php

$serverCode = <<<'CODE'
    $serverFlags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $ctx = stream_context_create(['ssl' => [
        'SNI_server_certs' => [
            "cs.php.net" => __DIR__ . "/sni_server_cs.pem",
            "uk.php.net" => __DIR__ . "/sni_server_uk.pem",
            "us.php.net" => __DIR__ . "/sni_server_us.pem"
        ]
    ]]);

    $server = stream_socket_server('tls://127.0.0.1:0', $errno, $errstr, $serverFlags, $ctx);
    phpt_notify_server_start($server);

    for ($i=0; $i < 3; $i++) {
        $conn = stream_socket_accept($server, 3);
        fwrite($conn, "HTTP/1.0 200 OK\r\n\r\nHello from server $i");
        fclose($conn);
    }

    phpt_wait();
CODE;

$proxyCode = <<<'CODE'
    function parse_sni_from_client_hello($data) {
        $sni = null;

        if (strlen($data) < 5 || ord($data[0]) != 0x16) return null;

        $session_id_len = ord($data[43]);
        $ptr = 44 + $session_id_len;

        // Cipher suites length
        $cipher_suites_len = (ord($data[$ptr]) << 8) | ord($data[$ptr+1]);
        $ptr += 2 + $cipher_suites_len;

        // Compression methods length
        $compression_methods_len = ord($data[$ptr]);
        $ptr += 1 + $compression_methods_len;

        // Extensions length
        if ($ptr + 2 > strlen($data)) return null;
        $extensions_len = (ord($data[$ptr]) << 8) | ord($data[$ptr+1]);
        $ptr += 2;

        $extensions_end = $ptr + $extensions_len;

        while ($ptr + 4 <= $extensions_end) {
            $ext_type = (ord($data[$ptr]) << 8) | ord($data[$ptr+1]);
            $ext_len = (ord($data[$ptr+2]) << 8) | ord($data[$ptr+3]);
            $ptr += 4;

            if ($ext_type === 0x00) { // SNI extension
                if ($ptr + 2 > strlen($data)) break;
                $name_list_len = (ord($data[$ptr]) << 8) | ord($data[$ptr+1]);
                $ptr += 2;

                if ($ptr + 3 > strlen($data)) break;
                $name_type = ord($data[$ptr]);
                $name_len = (ord($data[$ptr+1]) << 8) | ord($data[$ptr+2]);
                $ptr += 3;

                if ($name_type === 0) { // host_name type
                    $sni = substr($data, $ptr, $name_len);
                    break;
                }
            }

            $ptr += $ext_len;
        }

        return $sni;
    }

    $flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
    $server = stream_socket_server("tcp://127.0.0.1:0", $errornum, $errorstr, $flags);
    phpt_notify_server_start($server);

    for ($i=0; $i < 3; $i++) {
        $upstream = stream_socket_client("tcp://{{ ADDR }}", $errornum, $errorstr, 30, STREAM_CLIENT_CONNECT);
        stream_set_blocking($upstream, false);

        $conn = stream_socket_accept($server);
        stream_set_blocking($conn, true);

        // reading CONNECT request headers
        while (($line = fgets($conn)) !== false) {
            if (rtrim($line) === '') break; // empty line means end of headers
        }

        // successful CONNECT response
        fwrite($conn, "HTTP/1.0 200 Connection established\r\n\r\n");

        // tunnel data
        stream_set_blocking($conn, false);
        $firstRead = true;
        while (!feof($conn) && !feof($upstream)) {
            $clientData = fread($conn, 8192);
            if ($clientData !== false && $clientData !== '') {
                if ($firstRead) {
                    $sni = parse_sni_from_client_hello($clientData);
                    if ($sni !== null) {
                        file_put_contents(__DIR__ . "/bug74796_proxy_sni.log", $sni . "\n", FILE_APPEND);
                    }
                    $firstRead = false;
                }
                fwrite($upstream, $clientData);
            }

            $serverData = fread($upstream, 8192);
            if ($serverData !== false && $serverData !== '') {
                fwrite($conn, $serverData);
            }
        }
        fclose($conn);
        fclose($upstream);
        phpt_wait();
    }
CODE;

$clientCode = <<<'CODE'
    $clientCtx = stream_context_create([
        'ssl' => [
            'cafile' => __DIR__ . '/sni_server_ca.pem',
            'verify_peer' => true,
            'verify_peer_name' => true,
        ],
        "http" => [
            "proxy" => "tcp://{{ ADDR }}"
        ],
    ]);

    // servers
    $hosts = ["cs.php.net", "uk.php.net", "us.php.net"];
    foreach ($hosts as $host) {
        var_dump(file_get_contents("https://$host/", false, $clientCtx));
        var_dump(stream_context_get_options($clientCtx)['ssl']['peer_name'] ?? null);
        phpt_notify('proxy');
    }

    echo file_get_contents(__DIR__ . "/bug74796_proxy_sni.log");

    phpt_notify('server');
CODE;

include 'ServerClientTestCase.inc';
ServerClientTestCase::getInstance()->run($clientCode, [
    'server' => $serverCode,
    'proxy' => $proxyCode,
]);
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/bug74796_proxy_sni.log");
?>
--EXPECT--
string(19) "Hello from server 0"
NULL
string(19) "Hello from server 1"
NULL
string(19) "Hello from server 2"
NULL
cs.php.net
uk.php.net
us.php.net
