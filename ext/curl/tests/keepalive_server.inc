<?php
/* A minimal HTTP/1.1 server that keeps connections open, unlike the CLI server
 * started by server.inc, so that tests can observe libcurl's connection reuse.
 * Each response body is the number of connections accepted so far: two
 * responses with the same body were served on the same connection. */
function curl_keepalive_server_start(): string {
    $code = <<<'PHP'
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
    if ($server === false) {
        echo "error: $errstr\n";
        exit(1);
    }
    echo stream_socket_get_name($server, false), "\n";
    $clients = [];
    $buffers = [];
    $accepted = 0;
    while (true) {
        $read = $clients;
        $read[] = $server;
        $write = $except = null;
        if (stream_select($read, $write, $except, 60) < 1) {
            break;
        }
        foreach ($read as $socket) {
            if ($socket === $server) {
                $client = stream_socket_accept($server);
                $accepted++;
                $clients[(int) $client] = $client;
                $buffers[(int) $client] = '';
                continue;
            }
            $data = fread($socket, 8192);
            if ($data === '' || $data === false) {
                unset($clients[(int) $socket], $buffers[(int) $socket]);
                fclose($socket);
                continue;
            }
            $buffers[(int) $socket] .= $data;
            while (($end = strpos($buffers[(int) $socket], "\r\n\r\n")) !== false) {
                $buffers[(int) $socket] = substr($buffers[(int) $socket], $end + 4);
                $body = (string) $accepted;
                fwrite($socket, "HTTP/1.1 200 OK\r\nContent-Length: " . strlen($body) . "\r\n\r\n" . $body);
            }
        }
    }
    PHP;

    $php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
    $process = proc_open([$php, '-n', '-r', $code], [1 => ['pipe', 'w']], $pipes);
    $address = trim((string) fgets($pipes[1]));
    if (!preg_match('/^127\.0\.0\.1:\d+$/', $address)) {
        proc_terminate($process);
        die("Failed to start the keep-alive server: $address");
    }

    register_shutdown_function(function () use ($process, $pipes): void {
        proc_terminate($process);
        fclose($pipes[1]);
        proc_close($process);
    });

    return "http://$address";
}
