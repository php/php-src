<?php

// TODO: Move address/port info in here?
class CliServerInfo {
    public function __construct(
        public string $docRoot,
        public $processHandle,
    ) {}
}

function php_cli_server_start(
    ?string $code = 'echo "Hello world";',
    ?string $router = 'index.php',
    array $cmd_args = []
): CliServerInfo {
    $php_executable = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
    $error = null;

    // Create dedicated doc root to avoid index.php clashes between tests.
    $doc_root = __DIR__ . DIRECTORY_SEPARATOR . basename($_SERVER['PHP_SELF'], '.php');
    @mkdir($doc_root);

    if ($code) {
        file_put_contents($doc_root . '/' . ($router ?: 'index.php'), '<?php ' . $code . ' ?>');
    }

    $cmd = [$php_executable, '-t', $doc_root, '-n', ...$cmd_args, '-S', 'localhost:0'];
    if (!is_null($router)) {
        $cmd[] = $router;
    }

    $output_file = tempnam(sys_get_temp_dir(), 'cli_server_output');
    $output_file_fd = fopen($output_file, 'ab');
    if ($output_file_fd === false) {
        die(sprintf("Failed opening output file %s\n", $output_file));
    }

    $descriptorspec = array(
        0 => STDIN,
        1 => $output_file_fd,
        2 => $output_file_fd,
    );
    $handle = proc_open($cmd, $descriptorspec, $pipes, $doc_root, null, array("suppress_errors" => true));

    // First, wait for the dev server to declare itself ready.
    $bound = null;
    for ($i = 0; $i < 60; $i++) {
        usleep(50000); // 50ms per try
        $status = proc_get_status($handle);
        if (empty($status['running'])) {
            echo "Server failed to start\n";
            printf("Server output:\n%s\n", file_get_contents($output_file));
            proc_terminate($handle);
            exit(1);
        }

        $output = file_get_contents($output_file);
        if (preg_match('@PHP \S* Development Server \(https?://(.*?:\d+)\) started@', $output, $matches)) {
            $bound = $matches[1];
            break;
        }
    }
    if ($bound === null) {
        echo "Server did not output startup message";
        printf("Server output:\n%s\n", file_get_contents($output_file));
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
            $error = sprintf("Server stopped\nServer output:\n%s\n", file_get_contents($output_file));
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
        function($handle) use($router, $doc_root, $output_file) {
            proc_terminate($handle);
            $status = proc_get_status($handle);
            if ($status['exitcode'] !== -1 && $status['exitcode'] !== 0
                    && !($status['exitcode'] === 255 && PHP_OS_FAMILY == 'Windows')) {
                printf("Server exited with non-zero status: %d\n", $status['exitcode']);
                printf("Server output:\n%s\n", file_get_contents($output_file));
            }
            @unlink(__DIR__ . "/{$router}");
            remove_directory($doc_root);
        },
        $handle
    );

    // Define the same "constants" we previously did.
    $port = (int) substr($bound, strrpos($bound, ':') + 1);
    define("PHP_CLI_SERVER_HOSTNAME", "localhost");
    define("PHP_CLI_SERVER_PORT", $port);
    define("PHP_CLI_SERVER_ADDRESS", PHP_CLI_SERVER_HOSTNAME.":".PHP_CLI_SERVER_PORT);

    return new CliServerInfo($doc_root, $handle);
}

function php_cli_server_connect() {
    $timeout = 1.0;
    $fp = fsockopen(PHP_CLI_SERVER_HOSTNAME, PHP_CLI_SERVER_PORT, $errno, $errstr, $timeout);
    if (!$fp) {
        die("connect failed");
    }
    return $fp;
}

function remove_directory($dir) {
    if (is_dir($dir) === false) {
        return;
    }
    $files = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST
    );
    foreach ($files as $fileinfo) {
        $todo = ($fileinfo->isDir() ? 'rmdir' : 'unlink');
        $todo($fileinfo->getRealPath());
    }
    @rmdir($dir);
}

?>
