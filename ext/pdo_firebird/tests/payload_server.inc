<?php

function run_server(string $payloadFile): string {
    $cmd = [getenv("TEST_PHP_EXECUTABLE"), "-n", __DIR__ . "/payload_server.php", $payloadFile];
    $descriptorspec = array(
        0 => STDIN,
        1 => STDOUT,
        2 => ['pipe', 'w'],
    );
    $proc = proc_open($cmd, $descriptorspec, $pipes);

    // First, wait for the payload server to declare itself ready.
    $bound = null;
    stream_set_blocking($pipes[2], false);
    for ($i = 0; $i < 60; $i++) {
        usleep(50000); // 50ms per try
        $status = proc_get_status($proc);
        if (empty($status['running'])) {
            echo "Server is not running\n";
            proc_terminate($proc);
            exit(1);
        }
        while (($line = fgets($pipes[2])) !== false) {
            if (preg_match('/FB payload server listening on (.+)/', $line, $matches)) {
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
        proc_terminate($proc);
        exit(1);
    }

    // Now wait for a connection to succeed.
    // note: even when server prints 'FB payload server listening on localhost:12345'
    //       it might not be listening yet...need to wait until fsockopen() call returns
    $error = "Unable to connect to server\n";
    for ($i=0; $i < 60; $i++) {
        usleep(50000); // 50ms per try
        $status = proc_get_status($proc);
        $fp = fsockopen("tcp://$bound");
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
        proc_terminate($proc);
        exit(1);
    }

    register_shutdown_function(
        function($proc) {
            proc_terminate($proc);
            /* Wait for server to shutdown */
            for ($i = 0; $i < 60; $i++) {
                $status = proc_get_status($proc);
                if (!($status && $status['running'])) {
                    break;
                }
                usleep(50000);
            }
        },
        $proc
    );

    return $bound;
}
