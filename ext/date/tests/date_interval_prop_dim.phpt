--TEST--
Test DateInterval props with dimension handling
--FILE--
<?php
class Z extends DateInterval{}
$z = new Z('P2Y4DT6H8M');
$i = 0;
$z->prop[1]=10;
while ($i < 1026) {
	$z->prop[$i] = $i;
	$i++;
}
?>
==NOCRASH==
--EXPECT--
==NOCRASH==
