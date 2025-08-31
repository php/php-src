<?php declare(strict_types=1);

function curl_cli_server_start() {
    $php_executable = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
    $doc_root = __DIR__;
    $router = "responder/get.inc";
    $cmd = [$php_executable, '-t', $doc_root, '-n', '-S', 'localhost:0', $router];
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

    if ($fp) {
        fclose($fp);
    }

    if ($error) {
        echo $error;
        proc_terminate($handle);
        exit(1);
    }

    register_shutdown_function(
        function($handle) {
            proc_terminate($handle);
            /* Wait for server to shutdown */
            for ($i = 0; $i < 60; $i++) {
                $status = proc_get_status($handle);
                if (!($status && $status['running'])) {
                    break;
                }
                usleep(50000);
            }
        },
        $handle
    );

    return $bound;
}
