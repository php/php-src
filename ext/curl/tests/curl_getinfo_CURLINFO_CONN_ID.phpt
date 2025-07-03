--TEST--
Curlinfo CURLINFO_CONN_ID
--EXTENSIONS--
curl
--SKIPIF--
<?php
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x080200) die("skip: test works only with curl >= 8.2.0");
?>
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();
$port = (int) (explode(':', $host))[1];

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=get");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

$info = curl_getinfo($ch);
var_dump(isset($info['conn_id']));
var_dump($info['conn_id'] === -1);

$result = curl_exec($ch);

$info = curl_getinfo($ch);
var_dump(isset($info['conn_id']));
var_dump(is_int($info['conn_id']));
var_dump(curl_getinfo($ch, CURLINFO_CONN_ID) === $info['conn_id']);
var_dump(curl_getinfo($ch, CURLINFO_CONN_ID) === 0);

curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=get");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$result = curl_exec($ch);

$info = curl_getinfo($ch);
var_dump(isset($info['conn_id']));
var_dump(is_int($info['conn_id']));
var_dump(curl_getinfo($ch, CURLINFO_CONN_ID) === $info['conn_id']);
var_dump(curl_getinfo($ch, CURLINFO_CONN_ID) === 1);

$ch1=curl_init();
$ch2=curl_init();
$cmh=curl_multi_init();

foreach([$ch1, $ch2] as $ch) {
    curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=getpost&get_param=Curl%20Handle");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    $info = curl_getinfo($ch);
    var_dump(isset($info['conn_id']));
    var_dump($info['conn_id'] === -1);
    curl_multi_add_handle($cmh,$ch);
}

$running=0;
do {
    curl_multi_exec($cmh,$running);
} while ($running>0);

foreach([$ch1, $ch2] as $ch) {
    $result = curl_multi_getcontent($ch);
    $info = curl_getinfo($ch);
    var_dump(isset($info['conn_id']));
    var_dump(is_int($info['conn_id']));
    var_dump(curl_getinfo($ch, CURLINFO_CONN_ID) === $info['conn_id']);
    var_dump(curl_getinfo($ch, CURLINFO_CONN_ID) === 0);
}


?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

