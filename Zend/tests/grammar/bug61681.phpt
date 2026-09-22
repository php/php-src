--TEST--
Bug #61681: Malformed grammar
--FILE--
<?php
$la = "ooxx";

echo "${substr('laruence', 0, 2)}";

?>
--EXPECTF--
Deprecated: Using ${expr} (variable variables) in strings is deprecated, use {${expr}} instead in %s on line %d
ooxx
