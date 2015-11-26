--TEST--
strip_tags() function
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
?>
--EXPECT--
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
NEAT  STUFF
TESTS ?!!?!?!!!?!!
