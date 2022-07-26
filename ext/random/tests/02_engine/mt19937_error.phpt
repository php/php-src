--TEST--
Random: Engine: Mt19937: error pattern
--FILE--
<?php

new \Random\Engine\Mt19937(1234, 2);

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Random\Engine\Mt19937::__construct(): Argument #2 ($mode) mode must be MT_RAND_MT19937 or MT_RAND_PHP in %s:%d
Stack trace:
#0 %s(%d): Random\Engine\Mt19937->__construct(1234, 2)
#1 {main}
  thrown in %s on line %d
