--TEST--
Phar: bug #74386: Phar::__construct(): wrong number of parameters by reflection
--EXTENSIONS--
phar
--FILE--
<?php
$r = new ReflectionMethod(Phar::class, '__construct');
var_dump($r->getNumberOfRequiredParameters());
var_dump($r->getNumberOfParameters());

$r = new ReflectionMethod(PharData::class, '__construct');
var_dump($r->getNumberOfRequiredParameters());
var_dump($r->getNumberOfParameters());
?>
--EXPECT--
int(1)
int(3)
int(1)
int(4)
