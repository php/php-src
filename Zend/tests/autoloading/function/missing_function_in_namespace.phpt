--TEST--
Missing function in namespace
--FILE--
<?php

namespace zoq {
	quux();
}
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function zoq\quux() in %s
Stack trace:
#0 {main}
  thrown in %s on line %d
