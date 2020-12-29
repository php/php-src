--TEST--
Test CURLOPT_*SOCK
--SKIPIF--
<?php
if (!extension_loaded("curl")) exit("skip curl extension not loaded");
if (!extension_loaded("sockets")) exit("skip sockets extension not loaded");
?>
--FILE--
<?php
function opensocket($handler, $purpose, $address) {
    echo "opensocket", PHP_EOL;
	return socket_create($address['family'], $address['socktype'], $address['protocol']);
}

function sockopt($handler, $fd, $purpose) {
    echo "sockopt", PHP_EOL;
}

function closesocket($handler, $fd) {
    echo "closesocket", PHP_EOL;
	socket_close($fd);
}

include 'server.inc';
$host = curl_cli_server_start();

$url = "{$host}/get.php?test=get";
$ch = curl_init();

ob_start(); // start output buffering
curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use
curl_setopt($ch, CURLOPT_SOCKOPTFUNCTION, 'sockopt');
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, 'opensocket');
curl_setopt($ch, CURLOPT_CLOSESOCKETFUNCTION, 'closesocket');


$ok = curl_exec($ch);
curl_close($ch);
$curl_content = ob_get_contents();
ob_end_clean();

if($ok) {
	var_dump( $curl_content );
} else {
	echo "curl_exec returned false";
}
?>
===DONE===
--EXPECTF--
string(56) "opensocket
sockopt
Hello World!
Hello World!closesocket
"
===DONE===
