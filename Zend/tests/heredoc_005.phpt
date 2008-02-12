--TEST--
unbraced complex variable replacement test (heredoc)
--FILE--
<?php

require_once 'nowdoc.inc';

print <<<ENDOFHEREDOC
This is heredoc test #s $a, $b, $c['c'], and $d->d.

ENDOFHEREDOC;

$x = <<<ENDOFHEREDOC
This is heredoc test #s $a, $b, $c['c'], and $d->d.

ENDOFHEREDOC;

print "{$x}";

?>
--EXPECTF--
Parse error: syntax error, unexpected T_ENCAPSED_AND_WHITESPACE, expecting T_STRING or T_VARIABLE or T_NUM_STRING in %sheredoc_005.php on line 6
