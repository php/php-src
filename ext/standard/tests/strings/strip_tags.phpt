--TEST--
strip_tags() function
--POST--
--GET--
--FILE--
<?php
	echo strip_tags('NEAT <? cool < blah ?> STUFF');
	echo "\n";
	echo strip_tags('NEAT <? cool > blah ?> STUFF');
	echo "\n";
	echo strip_tags('NEAT <!-- cool < blah --> STUFF');
	echo "\n";
	echo strip_tags('NEAT <!-- cool > blah --> STUFF');
	echo "\n";
	echo strip_tags('NEAT <? echo \"\\\"\"?> STUFF');
	echo "\n";
	echo strip_tags('NEAT <? echo \'\\\'\'?> STUFF');
	echo "\n";
	echo strip_tags('TESTS ?!!?!?!!!?!!');
	echo "\n";
	echo strip_tags('<i>CAN-2004-<'.chr(0).'b>0595</i>', '<i>');
?>
--EXPECT--
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
TESTS ?!!?!?!!!?!!
<i>CAN-2004-0595</i>
