--TEST--
shadowing a global core function with a local version
--FILE--
<?php

use function foo\strlen;

require 'includes/foo_strlen.php';

var_dump(strlen('foo bar baz'));
echo "Done\n";

?>
--EXPECT--
int(4)
Done
