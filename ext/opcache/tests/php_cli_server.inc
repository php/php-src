<?php
define ("PHP_CLI_SERVER_HOSTNAME", "localhost");
define ("PHP_CLI_SERVER_PORT", 8964);
define ("PHP_CLI_SERVER_ADDRESS", PHP_CLI_SERVER_HOSTNAME.":".PHP_CLI_SERVER_PORT);

function php_cli_server_start($ini = "") {
    $php_executable = getenv('TEST_PHP_EXECUTABLE');
    $doc_root = __DIR__;

    $ini_array = preg_split('/\s+/', trim($ini));
    $ini_array = array_map(function($arg) {
        return trim($arg, '\'"');
    }, $ini_array);
    $cmd = [$php_executable, '-t', $doc_root, '-n', ...$ini_array, '-S', PHP_CLI_SERVER_ADDRESS];
    $descriptorspec = array(
        0 => STDIN,
        1 => STDOUT,
        2 => array("null"),
    );
    $handle = proc_open($cmd, $descriptorspec, $pipes, $doc_root, null, array("suppress_errors" => true));

    // note: even when server prints 'Listening on localhost:8964...Press Ctrl-C to quit.'
    //       it might not be listening yet...need to wait until fsockopen() call returns
    $error = "Unable to connect to server\n";
    for ($i=0; $i < 60; $i++) {
        usleep(50000); // 50ms per try
        $status = proc_get_status($handle);
        $fp = @fsockopen(PHP_CLI_SERVER_HOSTNAME, PHP_CLI_SERVER_PORT);
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

}
?>
