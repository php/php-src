<?php
ini_set('precision',14);
$tmp = pow(2,64);
$data = array( 
	'PHP',
	17=>'PHP: Hypertext Preprocessor',
	5=>'Test',
	'test'=>27,
	2147483647=>'test',
	"-2147483648"=>array('banana', 'orange'),
	'monkey',
	$tmp=>-1/3
);
?>
