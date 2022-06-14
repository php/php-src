--TEST--
curl_url_set() and CurlUrl::set()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
$url = curl_url();
curl_url_set($url, CURLUPART_HOST, 'www.example.com');
curl_url_set($url, CURLUPART_SCHEME, 'foobar', CURLU_NON_SUPPORT_SCHEME);
echo curl_url_get($url, CURLUPART_URL), PHP_EOL;

$url->set(CURLUPART_HOST, 'www.php.net');
$url->set(CURLUPART_SCHEME, 'foobar', CURLU_NON_SUPPORT_SCHEME);
echo $url->get(CURLUPART_URL);

?>
--EXPECT--
foobar://www.example.com/
foobar://www.php.net/
