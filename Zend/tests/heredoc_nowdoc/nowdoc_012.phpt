--TEST--
Test false labels
--FILE--
<?php

require_once 'nowdoc.inc';

$x = <<<'ENDOFNOWDOC'
This is a nowdoc test.
NOTREALLYEND;
Another line
NOTENDEITHER;
ENDOFNOWDOCWILLBESOON
Now let's finish it
ENDOFNOWDOC;
print "{$x}\n";

?>
--EXPECT--
This is a nowdoc test.
NOTREALLYEND;
Another line
NOTENDEITHER;
ENDOFNOWDOCWILLBESOON
Now let's finish it
