--TEST--
filter.default is deprecated
--EXTENSIONS--
filter
--INI--
filter.default=special_chars
--FILE--
<?php

echo "Done\n";

?>
--EXPECT--
Deprecated: The filter.default ini setting is deprecated in Unknown on line 0
Done
