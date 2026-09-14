--TEST--
Proxy-Authorization header removed from request after CONNECT tunnel
--EXTENSIONS--
openssl
--SKIPIF--
<?php require_once 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require_once 'server.inc';

$server = http_server_init($output);

if (is_resource($server)) {
    $conn = stream_socket_accept($server);

    /* Read CONNECT request */
    $req = '';
    while (!str_contains($req, "\r\n\r\n")) {
        $req .= fread($conn, 1024);
    }

    echo "CONNECT contains Proxy-Authorization: ";
    var_dump(stripos($req, 'Proxy-Authorization:') !== false);

    fwrite($conn, "HTTP/1.1 200 Connection established\r\n\r\n");
    fflush($conn);

    stream_context_set_option($conn, 'ssl', 'local_cert', __DIR__ . '/../../../openssl/tests/sni_server.pem');
    stream_socket_enable_crypto($conn, true, STREAM_CRYPTO_METHOD_TLS_SERVER) or die('fail TLS handshake');

    /* Read tunneled request */
    $req2 = '';
    while (!str_contains($req2, "\r\n\r\n")) {
        $req2 .= fread($conn, 1024);
    }

    /* Must be removed */
    echo "Proxied request contains Proxy-Authorization: ";
    var_dump(stripos($req2, 'Proxy-Authorization:') !== false);

    fwrite($conn,
        "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n"
    );

    exit;
}

$host = parse_url($server['uri'], PHP_URL_HOST);
$port = parse_url($server['uri'], PHP_URL_PORT);

$ctx = stream_context_create([
    'http' => [
        'proxy' => "tcp://$host:$port",
        'header' => [
            "Proxy-Authorization: Basic Zm9vOmJhcg==",
        ],
    ],
    'ssl' => [
        'verify_peer' => false,
        'verify_peer_name' => false,
    ],
]);

file_get_contents("https://www.php.net/", false, $ctx);

http_server_kill($server['pid']);
?>
--EXPECT--
CONNECT contains Proxy-Authorization: bool(true)
Proxied request contains Proxy-Authorization: bool(false)
