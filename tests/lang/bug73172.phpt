--TEST--
Bug #73172 parse error: Invalid numeric literal
--FILE--
<?php

setlocale(LC_ALL, 'fr_FR.utf8', 'fra');

include __DIR__ . DIRECTORY_SEPARATOR . "bug73172.inc";

?>
==DONE==
--EXPECT--
==DONE==
