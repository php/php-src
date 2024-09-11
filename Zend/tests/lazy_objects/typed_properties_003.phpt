--TEST--
Typed property assign op cached
--FILE--
<?php

class Test {
    public int $prop = 0;
}

function op($obj, $prop) {
    $obj->$prop += 1;
}
function pre($obj, $prop) {
    return ++$obj->$prop;
}
function post($obj, $prop) {
    return $obj->$prop++;
}

$reflector = new ReflectionClass(Test::class);
$obj = $reflector->newLazyProxy(function () {
    return new Test();
});

op($obj, 'prop');
op($obj, 'prop');

pre($obj, 'prop');
pre($obj, 'prop');

post($obj, 'prop');
post($obj, 'prop');

var_dump($obj);

?>
--EXPECTF--
lazy proxy object(Test)#%d (1) {
  ["instance"]=>
  object(Test)#%d (1) {
    ["prop"]=>
    int(6)
  }
}
