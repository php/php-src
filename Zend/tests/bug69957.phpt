--TEST--
Bug #69957 (Different ways of handling div/mod by zero)
--FILE--
<?php

try {
	$divisor = 0;
	$result = 1 / $divisor;
	var_dump($result);
} catch (Throwable $t){
	echo "Variable div\n";
	printf("Type: %s\n", get_class($t));
	printf("Message: %s\n", $t->getMessage());
}

try {
	$divisor = 0;
	$result = 1 % $divisor;
	var_dump($result);
} catch (Throwable $t){
	echo "\nVariable mod\n";
	printf("Type: %s\n", get_class($t));
	printf("Message: %s\n", $t->getMessage());
}

try {
	$result = 1 / 0;
	var_dump($result);
} catch (Throwable $t){
	echo "\nLiteral div\n";
	printf("Type: %s\n", get_class($t));
	printf("Message: %s\n", $t->getMessage());
}

try {
	$result = 1 % 0;
	var_dump($result);
} catch (Throwable $t){
	echo "\nLiteral mod\n";
	printf("Type: %s\n", get_class($t));
	printf("Message: %s\n", $t->getMessage());
}

try {
	$result = 1 / 0.0;
	var_dump($result);
} catch (Throwable $t){
	echo "\nDouble div\n";
	printf("Type: %s\n", get_class($t));
	printf("Message: %s\n", $t->getMessage());
}

try {
	$result = 1 % 0.0;
	var_dump($result);
} catch (Throwable $t){
	echo "\nDouble mod\n";
	printf("Type: %s\n", get_class($t));
	printf("Message: %s\n", $t->getMessage());
}

?>
--EXPECTF--
Warning: Division by zero in %sbug69957.php on line %d
float(INF)

Variable mod
Type: Exception
Message: Division by zero

Warning: Division by zero in %sbug69957.php on line %d
float(INF)

Literal mod
Type: Exception
Message: Division by zero

Warning: Division by zero in %sbug69957.php on line %d
float(INF)

Double mod
Type: Exception
Message: Division by zero

