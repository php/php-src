<?php
define ("PHP_CLI_SERVER_HOSTNAME", "localhost");

function php_cli_server_start($ini = "", $doc_root = null) {
    $php_executable = getenv('TEST_PHP_EXECUTABLE');
    if ($php_executable) {
        $php_executable = realpath(__DIR__ . '/../../../' . $php_executable) ?: realpath($php_executable) ?: $php_executable;
    } else {
        $php_executable = PHP_BINARY;
    }
    $doc_root ??= __DIR__;

    $ini_array = preg_split('/\s+/', trim($ini));
    $ini_array = array_map(function($arg) {
        return trim($arg, '\'"');
    }, $ini_array);
    $cmd = [$php_executable, '-t', $doc_root, '-n', ...$ini_array, '-S', PHP_CLI_SERVER_HOSTNAME . ':0'];
    $output_file = tempnam(sys_get_temp_dir(), 'opcache_cli_server_output');
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
    if ($handle === false) {
        printf("Failed to start server process\nServer output:\n%s\n", file_get_contents($output_file));
        @unlink($output_file);
        exit(1);
    }

    $bound = null;
    for ($i=0; $i < 60; $i++) {
        usleep(50000); // 50ms per try
        $status = proc_get_status($handle);
        // Failure, the server is no longer running
        if (!($status && $status['running'])) {
            printf("Server failed to start\nServer output:\n%s\n", file_get_contents($output_file));
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
        printf("Server did not output startup message\nServer output:\n%s\n", file_get_contents($output_file));
        proc_terminate($handle);
        exit(1);
    }

    $port = (int) substr($bound, strrpos($bound, ':') + 1);
    define ("PHP_CLI_SERVER_PORT", $port);
    define ("PHP_CLI_SERVER_ADDRESS", PHP_CLI_SERVER_HOSTNAME.":".PHP_CLI_SERVER_PORT);

    // note: even when server prints its startup message it might not be listening yet;
    //       wait until fsockopen() succeeds.
    $error = "Unable to connect to server\n";
    for ($i=0; $i < 60; $i++) {
        usleep(50000); // 50ms per try
        $status = proc_get_status($handle);
        $fp = @fsockopen("tcp://" . $bound);
        // Failure, the server is no longer running
        if (!($status && $status['running'])) {
            $error = sprintf("Server is not running\nServer output:\n%s\n", file_get_contents($output_file));
            break;
        }
        // Success, Connected to servers
        if ($fp) {
            $error = '';
            break;
        }
    }

    if ($fp) {
        fclose($fp);
    }

    if ($error) {
        echo $error;
        proc_terminate($handle);
        @unlink($output_file);
        exit(1);
    }

    register_shutdown_function(
        function($handle, $output_file) {
            proc_terminate($handle);
            /* Wait for server to shutdown */
            for ($i = 0; $i < 60; $i++) {
                $status = proc_get_status($handle);
                if (!($status && $status['running'])) {
                    if ($status && $status['exitcode'] !== -1 && $status['exitcode'] !== 0
                            && !($status['exitcode'] === 255 && PHP_OS_FAMILY === 'Windows')) {
                        printf("Server exited with non-zero status: %d\n", $status['exitcode']);
                        printf("Server output:\n%s\n", file_get_contents($output_file));
                    }
                    break;
                }
            usleep(50000);
            }
            @unlink($output_file);
        },
        $handle,
        $output_file
    );

}
?>
