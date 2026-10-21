<?php

/* Minimal single-shot raw HTTP server for OOM tests */
function oom_soap_http_server(string $response, string $repeat = '', int $times = 0): array
{
    $code = <<<'PHP'
        $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
        if (!$server) {
            fwrite(STDERR, "bind failed: $errstr\n");
            exit(1);
        }
        echo stream_socket_get_name($server, false), "\n";
        $conn = stream_socket_accept($server, 10);
        if ($conn === false) {
            exit(1);
        }
        $length = 0;
        while (($line = fgets($conn)) !== false && rtrim($line, "\r\n") !== '') {
            if (preg_match('/^Content-Length:\s*(\d+)/i', $line, $m)) {
                $length = (int) $m[1];
            }
        }
        while ($length > 0 && !feof($conn)) {
            $chunk = fread($conn, $length);
            if ($chunk === false || $chunk === '') {
                break;
            }
            $length -= strlen($chunk);
        }
        fwrite($conn, $argv[1]);
        /* Optionally keep appending $argv[2] so a test can make the server
         * produce far more than the client's memory limit without the parent
         * having to hold it, or pass it on the command line. */
        for ($i = 0; $i < (int) $argv[3]; $i++) {
            if (@fwrite($conn, $argv[2]) === false) {
                break;
            }
        }
        fflush($conn);
        fclose($conn);
        PHP;

    $proc = proc_open(
        [PHP_BINARY, '-n', '-d', 'memory_limit=-1', '-r', $code, $response, $repeat, (string) $times],
        [1 => ['pipe', 'w'], 2 => ['pipe', 'w']],
        $pipes,
    );
    if (!is_resource($proc)) {
        die('could not start the helper server');
    }
    $address = trim((string) fgets($pipes[1]));
    if ($address === '') {
        die('helper server did not report an address: ' . stream_get_contents($pipes[2]));
    }

    return ['proc' => $proc, 'uri' => 'http://' . $address];
}

function oom_soap_http_server_skipif(): void
{
    if (!function_exists('proc_open')) {
        die('skip proc_open() not available');
    }
    $server = @stream_socket_server('tcp://127.0.0.1:0');
    if (!$server) {
        die('skip stream_socket_server() failed');
    }
    fclose($server);
}
