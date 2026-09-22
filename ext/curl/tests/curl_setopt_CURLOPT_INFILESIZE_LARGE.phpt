--TEST--
Curl option CURLOPT_INFILESIZE_LARGE
--EXTENSIONS--
curl
--SKIPIF--
<?php
include 'skipif-nocaddy.inc';
?>
--FILE--
<?php
$ch = curl_init("https://localhost/show_upload_size");
$f = fopen(__FILE__,"r");
var_dump(curl_setopt_array($ch, [
    CURLOPT_RETURNTRANSFER => true,
    CURLOPT_UPLOAD => true,
    CURLOPT_INFILE => $f,
    CURLOPT_INFILESIZE => filesize(__FILE__),
    CURLOPT_CONNECTTIMEOUT => 3,
    CURLOPT_TIMEOUT => 3,
    CURLOPT_SSL_VERIFYHOST => 0,
    CURLOPT_SSL_VERIFYPEER => 0,
]));

var_dump(curl_exec($ch));

var_dump(curl_setopt($ch, CURLOPT_INFILESIZE, -1));
var_dump(curl_exec($ch));

var_dump(curl_setopt($ch, CURLOPT_INFILESIZE_LARGE, -1));
var_dump(curl_exec($ch));

rewind($f);
var_dump(curl_setopt($ch, CURLOPT_INFILESIZE, filesize(__FILE__)));
var_dump(curl_exec($ch));
var_dump(curl_setopt($ch, CURLOPT_INFILESIZE_LARGE, -1));
var_dump(curl_exec($ch));

rewind($f);
var_dump(curl_setopt($ch, CURLOPT_INFILESIZE_LARGE, filesize(__FILE__)));
var_dump(curl_exec($ch));

var_dump(curl_error($ch));

?>
--EXPECTF--
bool(true)
string(21) "Content-length: =%d="
bool(true)
string(18) "Content-length: =="
bool(true)
string(18) "Content-length: =="
bool(true)
string(21) "Content-length: =%d="
bool(true)
string(18) "Content-length: =="
bool(true)
string(21) "Content-length: =%d="
string(0) ""
