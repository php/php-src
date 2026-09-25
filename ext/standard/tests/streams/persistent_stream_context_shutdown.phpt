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

    public function stream_open($path, $mode, $options, &$opened_path): bool
    {
        return true;
    }

    public function stream_close(): void
    {
        $context = stream_context_create([
            'socket' => ['tcp_nodelay' => true],
        ]);
        $GLOBALS['late_persistent_client'] = stream_socket_client(
            $GLOBALS['late_context_address'],
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

$GLOBALS['late_context_address'] = 'tcp://' . stream_socket_get_name($server, false);
stream_wrapper_register('late-context', LateContextWrapper::class);
$trigger = fopen('late-context://trigger', 'r');

?>
--EXPECT--
int(0)
