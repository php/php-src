--TEST--
Bug #60471 Random "Invalid request (unexpected EOF)" using a router script
--SKIPIF--
<?php
include "skipif.inc";
if (!PHP_DEBUG) die("skip preconnect messages only show in debug mode");
?>
--FILE--
<?php
include "php_cli_server.inc";
$stderr_file = __DIR__ . "/bug60471.cli-server.stderr.out";
$server = php_cli_server_start('echo "Foo";', null, null, null, $stderr_file);

# Partial HTTP request (Unexpected EOF)
$sock = fsockopen(PHP_CLI_SERVER_HOSTNAME, PHP_CLI_SERVER_PORT);
fwrite($sock, "HEAD / HTTP/1.0\r\n");
fclose($sock);

# Mimmik a preconnect
$sock = fsockopen(PHP_CLI_SERVER_HOSTNAME, PHP_CLI_SERVER_PORT);
fclose($sock);

# Normal request
file_get_contents('http://'.PHP_CLI_SERVER_ADDRESS);

php_cli_server_stop($server);

echo file_get_contents($stderr_file);
?>
--CLEAN--
<?php
unlink(__DIR__ . "/bug60471.cli-server.stderr.out");

# We start the EXPECTF section with a preconnect message because
# php_cli_server_start() will check that the server has started
# with something that looks like a preconnect
?>
--EXPECTF--
[%s] %s:%d Accepted
[%s] %s:%d Closed without sending a request; it was probably just an unused speculative preconnection
[%s] %s:%d Closing
[%s] %s:%d Accepted
[%s] %s:%d Invalid request (Unexpected EOF)
[%s] %s:%d Closing
[%s] %s:%d Accepted
[%s] %s:%d Closed without sending a request; it was probably just an unused speculative preconnection
[%s] %s:%d Closing
[%s] %s:%d Accepted
[%s] %s:%d [200]: /
[%s] %s:%d Closing
