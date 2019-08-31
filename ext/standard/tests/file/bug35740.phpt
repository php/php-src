--TEST--
Bug #35740 (memory leak when including a directory)
--FILE--
<?php

include (__DIR__);

echo "Done\n";
?>
--EXPECTF--
Warning: include(%s): failed to open stream: %s in %s on line %d

Warning: include(): Failed opening '%s' for inclusion (include_path='%s') in %s on line %d
Done
