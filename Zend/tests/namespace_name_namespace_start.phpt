--TEST--
Cannot use "namespace\xyz" as namespace name, due to conflict with ns-relative names
--FILE--
<?php

namespace NAMEspace\xyz;

?>
--EXPECTF--
Parse error: syntax error, unexpected namespace-relative name "NAMEspace\xyz", expecting "{" in %s on line %d
