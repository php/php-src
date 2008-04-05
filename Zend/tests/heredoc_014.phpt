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
Parse error: syntax error, unexpected T_SL in %sheredoc_014.php on line %d
