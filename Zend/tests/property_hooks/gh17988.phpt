--TEST--
GH-17988: Incorrect handling of hooked properties without get hook in get_object_vars()
--FILE--
<?php

class C
{
    public string $prop {
        set => $value;
    }
}

$c = new C;
$c->prop = 42;

var_dump($c);
var_dump(get_object_vars($c));
var_export($c);
echo "\n";
var_dump(json_encode($c));
var_dump((array)$c);

?>
--EXPECTF--
object(C)#%d (1) {
  ["prop"]=>
  string(2) "42"
}
array(1) {
  ["prop"]=>
  string(2) "42"
}
\C::__set_state(array(
   'prop' => '42',
))
string(13) "{"prop":"42"}"
array(1) {
  ["prop"]=>
  string(2) "42"
}
