--TEST--
non-existent imported functions should not be looked up in the global table
--FILE--
<?php
use function foo\bar\baz;

require 'includes/global_baz.php';

var_dump(baz());

?>
--EXPECTF--
Fatal error: Call to undefined function foo\bar\baz() in %s on line %d
