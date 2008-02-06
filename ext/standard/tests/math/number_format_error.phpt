--TEST--
Test number_format() - wrong params test number_format()
--FILE--
<?php
number_format();
number_format(23,2,true);
number_format(23,2,true,false,36);
?>
--EXPECTF--

Warning: Wrong parameter count for number_format() in %s on line 2

Warning: Wrong parameter count for number_format() in %s on line 3

Warning: Wrong parameter count for number_format() in %s on line 4

