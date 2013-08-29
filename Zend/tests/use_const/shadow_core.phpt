--TEST--
shadowing a global core constant with a local version
--FILE--
<?php

require 'includes/foo_php_version.php';

use const foo\PHP_VERSION;

var_dump(PHP_VERSION);
echo "Done\n";

?>
--EXPECTF--
int(42)
Done
