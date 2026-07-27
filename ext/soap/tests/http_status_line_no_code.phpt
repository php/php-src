--TEST--
SoapClient: malformed HTTP status line without status code must not crash
--EXTENSIONS--
soap
--FILE--
<?php
$serverCode = <<<'CODE'
$server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr);
phpt_notify_server_start($server);
$conn = stream_socket_accept($server);
while (($line = fgets($conn)) !== false) {
	if ($line === "\r\n" || $line === "\n") {
		break;
	}
}
fwrite($conn, "HTTP/1.1\r\nContent-Type: text/xml\r\nContent-Length: 0\r\n\r\n");
fclose($conn);
CODE;

$clientCode = <<<'CODE'
$client = new SoapClient(null, [
	'location' => 'http://{{ ADDR }}',
	'uri' => 'http://testuri.org',
	'connection_timeout' => 3,
]);
var_dump($client->__doRequest('<x/>', 'http://{{ ADDR }}', 'T', 1));
CODE;

include sprintf('%s/../../openssl/tests/ServerClientTestCase.inc', __DIR__);
ServerClientTestCase::getInstance()->run($clientCode, $serverCode);
?>
--EXPECT--
string(0) ""
