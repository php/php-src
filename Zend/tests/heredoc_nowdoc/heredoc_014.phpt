--TEST--
Heredoc with double quotes syntax but missing second quote
--FILE--
<?php
$test = "foo";
$var = <<<"MYLABEL
test: $test
MYLABEL;
echo $var;
?>
--EXPECTF--
Parse error: %s in %sheredoc_014.php on line %d
