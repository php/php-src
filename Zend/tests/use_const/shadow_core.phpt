--TEST--
shadowing a global core constant with a local version
--FILE--
<?php

require 'includes/foo_php_version.inc';

use const foo\PHP_VERSION;

var_dump(PHP_VERSION);
echo "Done\n";

?>
--EXPECT--
int(42)
Done
