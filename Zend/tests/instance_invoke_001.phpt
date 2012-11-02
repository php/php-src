--TEST--
Instance invoke 
--FILE--
<?php

class Foo
{
    public function __invoke() {
        echo "okey";
    }
}

(new Foo())();

class Bar
{
    public function __invoke($name) {
        return $name;
    }
}

var_dump((new Bar())());
var_dump((new Bar())("okey"));

--EXPECTF--
okey
Warning: Missing argument 1 for Bar::__invoke(), called in %sinstance_invoke_001.php on line %d and defined in %sinstance_invoke_001.php on line %d

Notice: Undefined variable: name in %sinstance_invoke_001.php on line %d
NULL
string(4) "okey"
