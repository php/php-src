--TEST--
Curl option CURLOPT_SSH_HOSTKEYFUNCTION
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (getenv('SKIP_ONLINE_TESTS')) die('skip Online test');
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x075400) {
    exit("skip: test works only with curl >= 7.84.0");
}
?>
--FILE--
<?php

function hostkeyfunction($ch, int $keyType, string $key, int $keyLength) {
    return CURLKHMATCH_MISMATCH;
}

// GitHub doesn't actually support SFTP, it does get far enough that the host key callback is called.
$ch = curl_init('sftp://php@github.com/file.txt');
curl_setopt($ch, CURLOPT_SSH_HOSTKEYFUNCTION, 'hostkeyfunction');
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
var_dump(curl_exec($ch));
var_dump(curl_errno($ch) == CURLE_SSL_CACERT);

?>
--EXPECT--
bool(false)
bool(true)
