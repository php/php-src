--TEST--
Multiple use of @ operator must not fail to suppress diagnostic
--FILE--
<?php

$var = @@$non_existent;

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
