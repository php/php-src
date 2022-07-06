--TEST--
Heredoc with double quotes
--FILE--
<?php
$test = "foo";
$var = <<<"MYLABEL"
test: $test
MYLABEL;
echo $var;
?>
--EXPECT--
test: foo
