--TEST--
GH-8409: Error in socket creation when error handler does not clean persistent connection
--FILE--
<?php
set_error_handler(function (int $errno, string $errstring): never {
    trigger_error($errstring, E_USER_ERROR);
});

register_shutdown_function(function (): void {
    foreach (get_resources() as $res) {
        if (get_resource_type($res) === 'persistent stream') {
            echo "ERROR: persistent stream not closed\n";
            return;
        }
    }
    echo "OK: persistent stream closed\n";
});

stream_socket_client('tcp://9999.9999.9999.9999:9999', $error_code, $error_message, 0.2, STREAM_CLIENT_CONNECT | STREAM_CLIENT_PERSISTENT);

echo "ERROR: this should not be visible\n";
?>
--EXPECTF--
Fatal error: stream_socket_client(): %s in %sgh8409.php on line %d
OK: persistent stream closed
