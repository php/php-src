--TEST--
Bug #72629 (Caught exception assignment to variables ignores references)
--FILE--
<?php

$var = null;
$e = &$var;

try {
	throw new Exception;
} catch (Exception $e) { }

var_dump($var === $e);
--EXPECT--
bool(true)
