--TEST--
Cannot use "namespace" as namespace name, due to conflict with ns-relative names
--FILE--
<?php

namespace NAMEspace;

?>
--EXPECTF--
Fatal error: Cannot use 'NAMEspace' as namespace name in %s on line %d
