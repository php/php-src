<?php

// TODO: Move address/port info in here?
class CliServerInfo {
    public function __construct(
        public string $docRoot,
    ) {}
}

function php_cli_server_start(
    ?string $code = 'echo "Hello world";',
    ?string $router = 'index.php',
    array $cmd_args = []
): CliServerInfo {
    $php_executable = getenv('TEST_PHP_EXECUTABLE');
    $error = null;

    // Create dedicated doc root to avoid index.php clashes between tests.
    $doc_root = __DIR__ . '/' . basename($_SERVER['PHP_SELF'], '.php');
    @mkdir($doc_root);

    if ($code) {
        file_put_contents($doc_root . '/' . ($router ?: 'index.php'), '<?php ' . $code . ' ?>');
    }

    $cmd = [$php_executable, '-t', $doc_root, '-n', ...$cmd_args, '-S', 'localhost:0'];
    if (!is_null($router)) {
        $cmd[] = $router;
    }

    $descriptorspec = array(
        0 => STDIN,
        1 => STDOUT,
        2 => ['pipe', 'w'],
    );
    $handle = proc_open($cmd, $descriptorspec, $pipes, $doc_root, null, array("suppress_errors" => true));

    // First, wait for the dev server to declare itself ready.
    $bound = null;
    stream_set_blocking($pipes[2], false);
    for ($i = 0; $i < 60; $i++) {
        usleep(50000); // 50ms per try
        $status = proc_get_status($handle);
        if (empty($status['running'])) {
            echo "Server is not running\n";
            proc_terminate($handle);
            exit(1);
        }

        while (($line = fgets($pipes[2])) !== false) {
            if (preg_match('@PHP \S* Development Server \(https?://(.*?:\d+)\) started@', $line, $matches)) {
                $bound = $matches[1];
                // Now that we've identified the listen address, close STDERR.
                // Otherwise the pipe may clog up with unread log messages.
                fclose($pipes[2]);
                break 2;
            }
        }
    }
    if ($bound === null) {
        echo "Server did not output startup message";
        proc_terminate($handle);
        exit(1);
    }

    // Now wait for a connection to succeed.
    // note: even when server prints 'Listening on localhost:8964...Press Ctrl-C to quit.'
    //       it might not be listening yet...need to wait until fsockopen() call returns
    $error = "Unable to connect to server\n";
    for ($i=0; $i < 60; $i++) {
        usleep(50000); // 50ms per try
        $status = proc_get_status($handle);
        $fp = @fsockopen("tcp://$bound");
        // Failure, the server is no longer running
        if (!($status && $status['running'])) {
            $error = "Server is not running\n";
            break;
        }
        // Success, Connected to servers
        if ($fp) {
            $error = '';
            break;
        }
    }

    if ($error) {
        echo $error;
        proc_terminate($handle);
        exit(1);
    }

    register_shutdown_function(
        function($handle) use($router, $doc_root) {
            proc_terminate($handle);
            @unlink(__DIR__ . "/{$router}");
            @rmdir($doc_root);
        },
        $handle
    );

    // Define the same "constants" we previously did.
    $port = (int) substr($bound, strrpos($bound, ':') + 1);
    define("PHP_CLI_SERVER_HOSTNAME", "localhost");
    define("PHP_CLI_SERVER_PORT", $port);
    define("PHP_CLI_SERVER_ADDRESS", PHP_CLI_SERVER_HOSTNAME.":".PHP_CLI_SERVER_PORT);

    return new CliServerInfo($doc_root);
}

function php_cli_server_connect() {
    $timeout = 1.0;
    $fp = fsockopen(PHP_CLI_SERVER_HOSTNAME, PHP_CLI_SERVER_PORT, $errno, $errstr, $timeout);
    if (!$fp) {
        die("connect failed");
    }
    return $fp;
}

?>
