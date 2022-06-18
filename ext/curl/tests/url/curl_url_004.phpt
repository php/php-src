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
echo $url->set('https://www.php.net')->get(), PHP_EOL;
echo $url->set('foobar://www.php.net', CurlUrl::NON_SUPPORT_SCHEME)->get(), PHP_EOL;

?>
--EXPECT--
https://www.php.net/
foobar://www.php.net/
