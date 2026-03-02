--TEST--
Bug #62839 (curl_copy_handle segfault with CURLOPT_FILE)
--EXTENSIONS--
curl
--FILE--
<?php
$curl = curl_init();

$fd = tmpfile();
curl_setopt($curl, CURLOPT_FILE, $fd);

curl_copy_handle($curl);

echo 'DONE!';
?>
--EXPECT--
DONE!
