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
float(INF)

Variable mod
Type: DivisionByZeroError
Message: Modulo by zero
float(INF)

Literal mod
Type: DivisionByZeroError
Message: Modulo by zero
float(INF)

Double mod
Type: DivisionByZeroError
Message: Modulo by zero

