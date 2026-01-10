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
Parse error: %s in %sheredoc_005.php on line 6
