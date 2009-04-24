--TEST--
strip_tags() function, binary variant
--FILE--
<?php
	echo strip_tags(b'NEAT <? cool < blah ?> STUFF');
	echo "\n";
	echo strip_tags(b'NEAT <? cool > blah ?> STUFF');
	echo "\n";
	echo strip_tags(b'NEAT <!-- cool < blah --> STUFF');
	echo "\n";
	echo strip_tags(b'NEAT <!-- cool > blah --> STUFF');
	echo "\n";
	echo strip_tags(b'NEAT <? echo \"\\\"\"?> STUFF');
	echo "\n";
	echo strip_tags(b'NEAT <? echo \'\\\'\'?> STUFF');
	echo "\n";
	echo strip_tags(b'TESTS ?!!?!?!!!?!!');
	echo "\n";
?>
--EXPECT--
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
TESTS ?!!?!?!!!?!!
