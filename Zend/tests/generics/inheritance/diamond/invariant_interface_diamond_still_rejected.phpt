--TEST--
Diamond + invariant T: arity mismatch is the one remaining rejection at the diamond stage
--FILE--
<?php
interface Multi<A, B> {}
interface Alpha extends Multi<int, string> {}
interface Beta extends Multi<int, string, float> {}
PHP
?>
--EXPECTF--
Fatal error: %s on line %d
