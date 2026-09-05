--TEST--
PREG_THROW_ON_ERROR: PregException is a global, strict-properties Exception subclass
--FILE--
<?php

$r = new ReflectionClass(PregException::class);

var_dump($r->getName());
var_dump($r->getParentClass()->getName());
var_dump($r->isFinal());

try {
    $ex = new PregException('boom');
    $ex->dynamic = 1;
    echo "dynamic property allowed\n";
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(13) "PregException"
string(9) "Exception"
bool(false)
Cannot create dynamic property PregException::$dynamic
