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
Parse error: parse error in %sheredoc_014.php on line 3
