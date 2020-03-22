--TEST--
curl_copy_handle() allows to post CURLFile multiple times with curl_multi_exec()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$ch1 = curl_init();
curl_setopt($ch1, CURLOPT_SAFE_UPLOAD, 1);
curl_setopt($ch1, CURLOPT_URL, "{$host}/get.php?test=file");
// curl_setopt($ch1, CURLOPT_RETURNTRANSFER, 1);

$filename = __DIR__ . '/АБВ.txt';
file_put_contents($filename, "Test.");
$file = curl_file_create($filename);
$params = array('file' => $file);
var_dump(curl_setopt($ch1, CURLOPT_POSTFIELDS, $params));

$ch2 = curl_copy_handle($ch1);
$ch3 = curl_copy_handle($ch1);

$mh = curl_multi_init();
curl_multi_add_handle($mh, $ch1);
curl_multi_add_handle($mh, $ch2);
do {
    $status = curl_multi_exec($mh, $active);
    if ($active) {
        curl_multi_select($mh);
    }
} while ($active && $status == CURLM_OK);

curl_multi_remove_handle($mh, $ch1);
curl_multi_remove_handle($mh, $ch2);
curl_multi_remove_handle($mh, $ch3);
curl_multi_close($mh);
?>
===DONE===
--EXPECTF--
bool(true)
АБВ.txt|application/octet-stream|5АБВ.txt|application/octet-stream|5===DONE===
--CLEAN--
<?php
@unlink(__DIR__ . '/АБВ.txt');
?>
