--TEST--
Test url() function : Test path where control character is replaced with underscore
--CREDIT--
Sol Richardson <sr5732358@hotmail.com>
--FILE--
<?php
/* Prototype  : proto mixed parse_url(string url, [int url_component])
 * Description: Parse a URL and return its components
 * Source code: ext/standard/url.c
 * Alias to functions:
 */

//PHPAPI char *php_replace_controlchars_ex(char *str, size_t len) Line 76
echo "-- Replace Control Character With Underscore Test --\n";
var_dump(parse_url("\f", PHP_URL_SCHEME));

--EXPECTF--
-- Replace Control Character With Underscore Test --
NULL
