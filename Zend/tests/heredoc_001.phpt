--TEST--
basic heredoc syntax
--FILE--
<?php

require_once 'nowdoc.inc';

print <<<ENDOFHEREDOC
This is a heredoc test.

ENDOFHEREDOC;

$x = <<<ENDOFHEREDOC
This is another heredoc test.

ENDOFHEREDOC;

print "{$x}";

?>
--EXPECT--
This is a heredoc test.
This is another heredoc test.
