--TEST--
Testing calls to anonymous function
--FILE--
<?php 

for ($i = 0; $i < 10; $i++) {
	$a = create_function('', 'return '. $i .';');
	var_dump($a());
	
	$b = "\0lambda_". ($i + 1);
	var_dump($b());
}

?>
--EXPECT--
int(0)
int(0)
int(1)
int(1)
int(2)
int(2)
int(3)
int(3)
int(4)
int(4)
int(5)
int(5)
int(6)
int(6)
int(7)
int(7)
int(8)
int(8)
int(9)
int(9)
