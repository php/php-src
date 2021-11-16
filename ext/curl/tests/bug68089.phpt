--TEST--
Bug #68089 (NULL byte injection - cURL lib)
--EXTENSIONS--
curl
--FILE--
<?php
$url = "file:///etc/passwd\0http://google.com";
$ch = curl_init();

try {
    curl_setopt($ch, CURLOPT_URL, $url);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
Done
--EXPECT--
curl_setopt(): cURL option must not contain any null bytes
Done
