--TEST--
Feedback Validating error of function ReflectionExtension
--CREDITS--
Michel Sobreira <sobreira.michel@gmail.com>
--FILE--
 <?php

	$ext = new ReflectionExtension('gd');
	if($ext != true)
	{
		$ext->info();
	}
	else
	{
		echo "function does not exist";
	}

 ?>
--EXPECT--
function does not exist