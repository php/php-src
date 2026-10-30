--TEST--
PCRE2 callout feature is not supported.
--SKIPIF--
<?php
if (PCRE_VERSION_MAJOR >= 10 && PCRE_VERSION_MINOR >= 45) {
	die("skip callout feature is disable in PCRE2 10.45 and above");
}
?>
--FILE--
<?php

$subject = 'hello';
$pattern = '/a(?C1)b(?C2)c/';

var_dump(preg_match($pattern, $subject));

?>
--EXPECT--
int(0)
