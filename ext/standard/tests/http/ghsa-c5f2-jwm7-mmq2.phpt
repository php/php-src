--TEST--
GHSA-c5f2-jwm7-mmq2 (Configuring a proxy in a stream context might allow for CRLF injection in URIs)
--INI--
allow_url_fopen=1
--CONFLICTS--
server
--FILE--
<?php
$serverCode = <<<'CODE'
echo $_SERVER['REQUEST_URI'];
CODE;

include __DIR__."/../../../../sapi/cli/tests/php_cli_server.inc";
php_cli_server_start($serverCode, null, []);

$host = PHP_CLI_SERVER_ADDRESS;
$userinput = "index.php HTTP/1.1\r\nHost: $host\r\n\r\nGET /index2.php HTTP/1.1\r\nHost: $host\r\n\r\nGET /index.php";
$context = stream_context_create(['http' => ['proxy' => 'tcp://' . $host, 'request_fulluri' => true]]);
echo file_get_contents("http://$host/$userinput", false, $context);
?>
--EXPECTF--
Warning: file_get_contents(http://localhost:%d/index.php HTTP/1.1
Host: localhost:%d

GET /index2.php HTTP/1.1
Host: localhost:%d

GET /index.php): Failed to open stream: HTTP wrapper full URI path does not allow CR or LF characters in %s on line %d
