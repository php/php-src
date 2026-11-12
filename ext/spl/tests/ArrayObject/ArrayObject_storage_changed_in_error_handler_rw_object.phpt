--TEST--
ArrayObject: an error handler can replace an object storage during a read-write access to an unset property
--FILE--
<?php
class C {
    public $p = 1;
    public $pad1, $pad2;
}

$o = new C();
unset($o->p);
$ao = @new ArrayObject($o, ArrayObject::ARRAY_AS_PROPS);
unset($o);

set_error_handler(function ($errno, $errstr) use ($ao) {
    echo $errstr, "\n";
    $ao->exchangeArray(['q' => 1]);
    return true;
});
$ao->p .= 'x';
var_dump($ao->getArrayCopy());
?>
--EXPECT--
Undefined array key "p"
array(2) {
  ["q"]=>
  int(1)
  ["p"]=>
  string(1) "x"
}
