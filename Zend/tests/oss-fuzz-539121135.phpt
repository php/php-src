--TEST--
OSS-Fuzz-539121135: Use-of-uninitialized-value ast->lineno
--FILE--
<?php

namespace {}

''

?>
--EXPECTF--
Fatal error: No code may exist outside of namespace {} in %s on line %d
