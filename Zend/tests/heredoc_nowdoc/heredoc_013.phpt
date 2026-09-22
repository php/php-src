--TEST--
Heredoc with double quotes and wrong prefix
--FILE--
<?php
$test = "foo";
$var = prefix<<<"MYLABEL"
test: $test
MYLABEL;
echo $var;
?>
--EXPECTF--
Parse error: %s in %sheredoc_013.php on line %d
