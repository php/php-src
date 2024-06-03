--TEST--
Parse invalid URL
--EXTENSIONS--
url
--FILE--
<?php

try {
    \Url\UrlParser::parseUrl("192.168/contact.html");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    \Url\UrlParser::parseUrlArray("192.168/contact.html");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
Url\UrlParser::parseUrl(): Argument #1 ($url) is not a valid URL
Url\UrlParser::parseUrlArray(): Argument #1 ($url) is not a valid URL
