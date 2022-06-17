--TEST--
CurlUrl::set() with and without flags
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
$url = new CurlUrl();

$url->set(CURLUPART_HOST, 'www.php.net');
$url->set(CURLUPART_SCHEME, 'foobar', CURLU_NON_SUPPORT_SCHEME);
echo $url->get(CURLUPART_URL);

?>
--EXPECT--
foobar://www.php.net/
