--TEST--
#[\Deprecated]: Using the value of a deprecated constant in a constant expression.
--FILE--
<?php

#[\Deprecated("prefix")]
const PREFIX = "prefix";

#[\Deprecated("suffix")]
const SUFFIX = "suffix";

const CONSTANT = PREFIX . SUFFIX;

var_dump(CONSTANT);

?>
--EXPECTF--
Deprecated: Constant PREFIX is deprecated, prefix in %s on line %d

Deprecated: Constant SUFFIX is deprecated, suffix in %s on line %d
string(12) "prefixsuffix"
