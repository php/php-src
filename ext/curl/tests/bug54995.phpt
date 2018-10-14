--TEST--
Bug #54995 (Missing CURLINFO_RESPONSE_CODE support)
--SKIPIF--
<?php
include 'skipif.inc';

if ($curl_version['version_number'] > 0x070a08) {
	exit("skip: tests works a versions of curl >= 7.10.8");
}
?>
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.php");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

var_dump(curl_getinfo($ch, CURLINFO_HTTP_CODE) == curl_getinfo($ch, CURLINFO_RESPONSE_CODE));

curl_exec($ch);
curl_close($ch);

?>
--EXPECTF--
bool(true)
