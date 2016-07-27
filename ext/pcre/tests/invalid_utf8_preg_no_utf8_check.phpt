--TEST--
preg_match() accepts invalid UTF8 when using PREG_NO_UTF8_CHECK
--SKIPIF--
<?php
if (@preg_match('/./u', '') === false) {
	die('skip no utf8 support in PCRE library');
}
?>
--FILE--
<?php

$string = urldecode("Gültig%e4");
$result = preg_match("/gültig(.+)/iu", $string, $matches, PREG_NO_UTF8_CHECK);

// match should succeed,
var_dump($result);

// without pcre errors,
var_dump(preg_last_error());

// and the invalid character should be captured,
// although we can't expect anything about it after that,
// since PCRE's behavior is undefined in this case
var_dump(count($matches));

?>
--EXPECT--
int(1)
int(0)
int(2)

