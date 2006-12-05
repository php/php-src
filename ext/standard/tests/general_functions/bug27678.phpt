--TEST--
Bug #27678 (number_format() crashes with large numbers)
--FILE--
<?php

number_format(1e80, 0, '', ' ');
number_format(1e300, 0, '', ' ');
number_format(1e320, 0, '', ' ');
number_format(1e1000, 0, '', ' ');

echo "Done\n";
?>
--EXPECT--	
Done
