--TEST--
shadowing a global core constant with a local version
--FILE--
<?php
use const foo\PHP_VERSION;

require 'includes/foo_php_version.php';


var_dump(PHP_VERSION);
echo "Done\n";

?>
--EXPECTF--
int(42)
Done
