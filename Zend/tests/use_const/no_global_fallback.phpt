--TEST--
non-existent imported constants should not be looked up in the global table
--FILE--
<?php

require 'includes/global_baz.inc';

use const foo\bar\baz;
var_dump(baz);

?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant 'foo\bar\baz' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
