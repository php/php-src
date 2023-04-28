--TEST--
GH-11146: The built-in webserver will add ";charset=UTF-8" to the end of the Content-Type response header of the SSE response.
--SKIPIF--
<?php
if (php_sapi_name() != "cli") {
    die("skip CLI only");
}
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(null, null);

header('Content-Type: text/event-stream');
for ($i = 0; $i < 3; ++$i) {
    echo 'data: ' . $i . "\n\n";
}
?>
--EXPECT--
data: 0

data: 1

data: 2
