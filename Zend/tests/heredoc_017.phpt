--TEST--
Testinh heredoc syntax
--FILE--
<?php 

$a = <<<A
	A;
;
 A;
\;
A;

var_dump(strlen($a) == 12);

?>
--EXPECT--
bool(true)
