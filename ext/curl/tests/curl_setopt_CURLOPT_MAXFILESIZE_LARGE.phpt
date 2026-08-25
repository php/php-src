--TEST--
Curl option CURLOPT_MAXFILESIZE_LARGE
--SKIPIF--
<?php
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x080400) {
    // Earlier curl has this option, but it only works
    // when the response has a Content-Length header.
    exit("skip: test works only with curl >= 8.4.0");
}
?>
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

echo "Body larger than CURLOPT_MAXFILESIZE_LARGE\n";
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_MAXFILESIZE_LARGE, 10);
$r = curl_exec($ch);
var_dump($r);
var_dump(curl_errno($ch) === CURLE_FILESIZE_EXCEEDED);

echo "Body smaller than CURLOPT_MAXFILESIZE_LARGE\n";
curl_setopt($ch, CURLOPT_MAXFILESIZE_LARGE, 30);
$r = curl_exec($ch);
var_dump(strlen($r));

echo "Limit disabled by setting CURLOPT_MAXFILESIZE_LARGE to 0\n";
curl_setopt($ch, CURLOPT_MAXFILESIZE_LARGE, 0);
$r = curl_exec($ch);
var_dump(strlen($r));

echo "Negative value for CURLOPT_MAXFILESIZE_LARGE\n";
$ok = curl_setopt($ch, CURLOPT_MAXFILESIZE_LARGE, -1);
var_dump($ok);
var_dump(curl_errno($ch) === CURLE_BAD_FUNCTION_ARGUMENT);

echo "Negative value not set, CURLOPT_MAXFILESIZE_LARGE is still 0\n";
$r = curl_exec($ch);
var_dump(strlen($r));

?>
--EXPECTF--
Body larger than CURLOPT_MAXFILESIZE_LARGE
bool(false)
bool(true)
Body smaller than CURLOPT_MAXFILESIZE_LARGE
int(25)
Limit disabled by setting CURLOPT_MAXFILESIZE_LARGE to 0
int(25)
Negative value for CURLOPT_MAXFILESIZE_LARGE
bool(false)
bool(true)
Negative value not set, CURLOPT_MAXFILESIZE_LARGE is still 0
int(25)
