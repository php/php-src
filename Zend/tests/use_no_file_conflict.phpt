--TEST--
Use conflicts should not occur across files
--FILE--
<?php

require __DIR__ . '/use_no_file_conflict_1.inc';
require __DIR__ . '/use_no_file_conflict_2.inc';

?>
===DONE===
--EXPECT--
===DONE===
