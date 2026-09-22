--TEST--
empty doc test (heredoc)
--FILE--
<?php

require_once 'nowdoc.inc';

print <<<ENDOFHEREDOC
ENDOFHEREDOC;

$x = <<<ENDOFHEREDOC
ENDOFHEREDOC;

print "{$x}";

?>
--EXPECT--
