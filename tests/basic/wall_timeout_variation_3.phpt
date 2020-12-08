--TEST--
Unsafe shutdown when hard timeout is reached
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
include __DIR__ . "/../../sapi/cli/tests/skipif.inc";
?>
--INI--
allow_url_fopen=1
hard_timeout=1
--FILE--
<?php

require __DIR__ . "/../../sapi/cli/tests/php_cli_server.inc";

php_cli_server_start("sleep(3);");

ini_set("max_execution_time", 1);
ini_set("max_execution_wall_time", 1);
ini_set("default_socket_timeout", 1);

register_shutdown_function(function () {
    echo "Unreachable";
});

@file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS);

?>
--EXPECTF--
Fatal error: Maximum execution wall-time of 1+1 seconds exceeded (terminated) in %s on line %d
