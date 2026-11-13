--TEST--
PCRE2 callout feature is not supported and disabled if pcre2 lib permits it.
--SKIPIF--
<?php
if (!(PCRE_VERSION_MAJOR >= 10 && PCRE_VERSION_MINOR >= 45)) {
	die("skip needs 10.45 or above");
}
?>
--FILE--
<?php

$subject = 'hello';
$pattern = '/a(?C1)b(?C2)c/';

var_dump(preg_match($pattern, $subject));

?>
--EXPECTF--
Warning: preg_match(): Compilation failed: using callouts is disabled by the application at offset 4 in %s on line %d
bool(false)
