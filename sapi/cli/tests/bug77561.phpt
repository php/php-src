--TEST--
Bug #77561: Shebang line not stripped for non-primary script
--FILE--
<?php

require __DIR__ . '/bug77561.inc';

?>
--EXPECT--
Test
