--TEST--
Verifying that the ReflectionExtension function is validating the parameters correctly
--CREDITS--
Michel Sobreira <sobreira.michel@gmail.com>
--FILE--
 <?php

	$ext = new ReflectionExtension('gd');
	if($ext == true)
	{
		echo "correctly";
	}
	else
	{
		echo "function does not exist";
	}

 ?>
--EXPECT--
correctly