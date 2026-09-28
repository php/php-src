--TEST--
Persistent stream contexts created during resource shutdown are detached
--FILE--
<?php

if (($argv[1] ?? null) !== 'child') {
    // Wait for the trigger's final status because the crash occurs after stdout closes.
    $process = proc_open(
        [PHP_BINARY, '-n', __FILE__, 'child'],
        [
            0 => ['pipe', 'r'],
            1 => ['pipe', 'w'],
            2 => ['pipe', 'w'],
        ],
        $pipes,
    );
    fclose($pipes[0]);
    stream_get_contents($pipes[1]);
    fclose($pipes[1]);
    stream_get_contents($pipes[2]);
    fclose($pipes[2]);
    var_dump(proc_close($process));
    return;
}

final class LateContextWrapper
{
    public $context;
    private string $address;

    public function stream_open($path, $mode, $options, &$opened_path): bool
    {
        $this->address = substr($path, strlen('late-context://'));
        return true;
    }

    public function stream_close(): void
    {
        $context = stream_context_create([
            'socket' => ['tcp_nodelay' => true],
        ]);
        stream_socket_client(
            $this->address,
            $errno,
            $error,
            1,
            STREAM_CLIENT_CONNECT | STREAM_CLIENT_PERSISTENT,
            $context,
        );
    }
}

$server = stream_socket_server('tcp://127.0.0.1:0', $errno, $error);
if (!$server) {
    die("server failed: $error ($errno)\n");
}

stream_wrapper_register('late-context', LateContextWrapper::class);
$trigger = fopen('late-context://tcp://' . stream_socket_get_name($server, false), 'r');

?>
--EXPECT--
int(0)
