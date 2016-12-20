<?php

define ("PHP_CURL_SERVER_HOSTNAME", "localhost");
define ("PHP_CURL_SERVER_PORT", 8964);
define ("PHP_CURL_SERVER_ADDRESS", PHP_CURL_SERVER_HOSTNAME.":".PHP_CURL_SERVER_PORT);

function curl_cli_server_start() {
    if(getenv('PHP_CURL_HTTP_REMOTE_SERVER')) {
        return getenv('PHP_CURL_HTTP_REMOTE_SERVER');
    }
    
    $php_executable = getenv('TEST_PHP_EXECUTABLE');
    $doc_root = __DIR__;
    $router = "responder/get.php";

    $descriptorspec = array(
        0 => STDIN,
        1 => STDOUT,
        2 => STDERR,
    );

    if (substr(PHP_OS, 0, 3) == 'WIN') {
        $cmd = "{$php_executable} -t {$doc_root} -n -S " . PHP_CURL_SERVER_ADDRESS;
        $cmd .= " {$router}";
        $handle = proc_open(addslashes($cmd), $descriptorspec, $pipes, $doc_root, NULL, array("bypass_shell" => true,  "suppress_errors" => true));
    } else {
        $cmd = "exec {$php_executable} -t {$doc_root} -n -S " . PHP_CURL_SERVER_ADDRESS;
        $cmd .= " {$router}";
        $cmd .= " 2>/dev/null";

        $handle = proc_open($cmd, $descriptorspec, $pipes, $doc_root);
    }
	
    // note: even when server prints 'Listening on localhost:8964...Press Ctrl-C to quit.'
    //       it might not be listening yet...need to wait until fsockopen() call returns
    $error = "Unable to connect to servers\n";
    for ($i=0; $i < 60; $i++) {
        usleep(25000); // 25ms per try
        $status = proc_get_status($handle);
        $fp = @fsockopen(PHP_CURL_SERVER_HOSTNAME, PHP_CURL_SERVER_PORT);
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
        proc_close($handle);
        exit(1);
    }

    register_shutdown_function(
        function($handle) use($router) {
            proc_terminate($handle);
        },
        $handle
		);

    return PHP_CURL_SERVER_ADDRESS;
}

