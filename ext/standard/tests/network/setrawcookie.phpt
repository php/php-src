--TEST--
bool setrawcookie ( string $name [, string $value [, int $expire = 0 [, string $path [, string $domain [, bool $secure = false [, bool $httponly = false ]]]]]] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$cookieName = "cookieRAW";
$cookieValue = "value-php-is-better-than-python";
$cookieExpire = time()+3600;
$cookiePath = "/path/";
$cookieDomain = "domain.tld";

var_dump(setrawcookie($cookieName, $cookieValue, $cookieExpire, $cookiePath, $cookieDomain, false, false));
?>
--EXPECT--
bool(true)
