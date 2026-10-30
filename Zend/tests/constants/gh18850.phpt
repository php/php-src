--TEST--
GH-18850: Repeated inclusion of file with __halt_compiler() triggers "Constant already defined" warning
--FILE--
<?php

require __DIR__ . '/gh18850.inc';
require __DIR__ . '/gh18850.inc';

?>
--EXPECT--
int(62)
int(62)
