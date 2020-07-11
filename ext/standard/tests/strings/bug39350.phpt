--TEST--
Bug #39350 (implode/join() crash on empty input strings)
--FILE--
<?php

implode('', array(null));
implode('', array(false));
implode('', array(""));

echo "Done\n";
?>
--EXPECT--
Done
