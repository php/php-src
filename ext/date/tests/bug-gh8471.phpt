--TEST--
Bug GH-8471: Segmentation fault when converting immutable and mutable DateTime instances created using reflection
--FILE--
<?php
$reflection = new ReflectionClass('\DateTime');

$mutable = $reflection->newInstanceWithoutConstructor();
try {
	$immutable = \DateTimeImmutable::createFromMutable($mutable);
} catch (Throwable $t) {
	echo $t->getMessage(), "\n";
}


$reflection = new ReflectionClass('\DateTime');

$mutable = $reflection->newInstanceWithoutConstructor();
try {
	$immutable = \DateTimeImmutable::createFromInterface($mutable);
} catch (Throwable $t) {
	echo $t->getMessage(), "\n";
}


$reflection = new ReflectionClass('\DateTimeImmutable');

$immutable = $reflection->newInstanceWithoutConstructor();
try {
	$mutable = \DateTime::createFromImmutable($immutable);
} catch (Throwable $t) {
	echo $t->getMessage(), "\n";
}


$reflection = new ReflectionClass('\DateTimeImmutable');

$immutable = $reflection->newInstanceWithoutConstructor();
try {
	$mutable = \DateTime::createFromInterface($immutable);
} catch (Throwable $t) {
	echo $t->getMessage(), "\n";
}


?>
--EXPECTF--
The DateTime object has not been correctly initialized by its constructor
The DateTimeInterface object has not been correctly initialized by its constructor
The DateTimeImmutable object has not been correctly initialized by its constructor
The DateTimeInterface object has not been correctly initialized by its constructor
