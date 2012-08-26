--TEST--
Getting the right type hinting information
--FILE--
<?php
$c = new ReflectionClass("ReflectionParameter");

// Method with type info
$m = $c->getMethod("export");
$p = $m->getParameters();
foreach ($p as $i)
	var_dump($i->getTypeHint());
echo "\n";

// Other method with type info
$m = $c->getMethod("__construct");
$p = $m->getParameters();
foreach ($p as $i)
	var_dump($i->getTypeHint());
echo "\n";

// Function with missing type info (at least for now)
$f = new ReflectionFunction("urlencode");
$p = $f->getParameters();
foreach ($p as $i)
	var_dump($i->getTypeHint());
?>
--EXPECT--
string(6) "string"
string(6) "string"
string(7) "boolean"

string(6) "string"
string(6) "string"

NULL
