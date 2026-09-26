--TEST--
PCRE2 callout feature is not supported.
--FILE--
<?php

$subject = 'hello';
$pattern = '/a(?C1)b(?C2)c/';

var_dump(preg_match($pattern, $subject));

?>
--EXPECT--
int(0)
