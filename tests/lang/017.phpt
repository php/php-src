--TEST--
Testing user-defined function falling out of an If into another
--POST--
--GET--
--FILE--
<?php $a = 1;
old_function Test $a (
	if($a<3):
		return(3);
	endif;
);

if($a < Test($a)):
	echo "$a\n";
	$a++;
endif?>
--EXPECT--
1
