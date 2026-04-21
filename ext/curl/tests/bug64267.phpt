--TEST--
Bug #64267 (CURLOPT_INFILE doesn't allow reset)
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
echo "TEST\n";

$host = curl_cli_server_start();
$c = curl_init($host);
$f = fopen(__FILE__,"r");
var_dump(curl_setopt_array($c, [
    CURLOPT_RETURNTRANSFER => true,
    CURLOPT_UPLOAD => true,
    CURLOPT_INFILE => $f,
    CURLOPT_INFILESIZE => filesize(__FILE__),
    CURLOPT_CONNECTTIMEOUT => 3,
    CURLOPT_TIMEOUT => 3,
]));
fclose($f);
var_dump(curl_setopt_array($c, [
    CURLOPT_UPLOAD => false,
    CURLOPT_INFILE => null,
    CURLOPT_INFILESIZE => 0,
]));
curl_exec($c);
var_dump(curl_getinfo($c, CURLINFO_RESPONSE_CODE));
?>
--EXPECT--
TEST
bool(true)
bool(true)
int(200)
