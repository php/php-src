--TEST--
Testing heredoc with tabs before identifier
--FILE--
<?php

$heredoc = <<<	A

foo

	A;
A;

var_dump(strlen($heredoc) == 9);

?>
--EXPECT--
bool(true)
