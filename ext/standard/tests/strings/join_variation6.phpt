--TEST--
Test join() function : usage variations - binary safe
--FILE--
<?php
/*
 * check the working of join() when given binary input given
*/

echo "*** Testing join() : usage variationsi - binary safe ***\n";

$glues = array(
  ",".chr(0)." ",
  ", "
);

$pieces = array("Red", "Green", "White", 1);
var_dump( join($glues[0], $pieces) );
var_dump( join($glues[1], $pieces) );

echo "Done\n";
?>
--EXPECTF--
*** Testing join() : usage variationsi - binary safe ***
string(23) "Red,%0 Green,%0 White,%0 1"
string(20) "Red, Green, White, 1"
Done
