--TEST--
Bug #80595 (Resetting POSTFIELDS to empty array breaks request)
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init();
curl_setopt_array($ch, [
    CURLOPT_RETURNTRANSFER => true,
    CURLOPT_POST           => true,
    CURLOPT_URL            => "{$host}/get.inc?test=post",
]);

curl_setopt($ch, CURLOPT_POSTFIELDS, ['foo' => 'bar']);
var_dump(curl_exec($ch));

curl_setopt($ch, CURLOPT_POSTFIELDS, []);
var_dump(curl_exec($ch));
?>
--EXPECT--
string(43) "array(1) {
  ["foo"]=>
  string(3) "bar"
}
"
string(13) "array(0) {
}
"
