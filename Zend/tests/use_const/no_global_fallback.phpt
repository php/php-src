--TEST--
non-existent imported constants should not be looked up in the global table
--FILE--
<?php

require 'includes/global_baz.php';

use const foo\bar\baz;
var_dump(baz);

?>
--EXPECTF--
Fatal error: Undefined constant 'foo\bar\baz' in %s on line %d
