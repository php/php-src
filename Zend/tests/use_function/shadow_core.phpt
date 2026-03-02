--TEST--
shadowing a global core function with a local version
--FILE--
<?php

require 'includes/foo_strlen.inc';

use function foo\strlen;

var_dump(strlen('foo bar baz'));
echo "Done\n";

?>
--EXPECT--
int(4)
Done
