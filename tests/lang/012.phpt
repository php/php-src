--TEST--
Testing stack after early function return
--POST--
--GET--
--FILE--
<?php 
old_function F ( 
	if(1):
		return("Hello");
	endif;
);

$i=0;
while($i<2):
	echo F();
	$i++;
endwhile;
?>
--EXPECT--
HelloHello
