--TEST--
A SensitiveParameterValue keeps the inner value secret.
--FILE--
<?php

$v = new SensitiveParameterValue('secret');

echo "# var_dump() / debug_zval_dump() / print_r()", PHP_EOL;
var_dump($v);
debug_zval_dump($v);
print_r($v);
echo PHP_EOL;

echo "# var_export()", PHP_EOL;
echo var_export($v, true), PHP_EOL;
echo PHP_EOL;

echo "# (array) / json_encode()", PHP_EOL;
var_dump((array)$v);
var_dump(json_encode($v));
echo PHP_EOL;

echo "# ->getValue()", PHP_EOL;
var_dump($v->getValue());

?>
--EXPECTF--
# var_dump() / debug_zval_dump() / print_r()
object(SensitiveParameterValue)#%d (0) {
}
object(SensitiveParameterValue)#%d (%d) refcount(%d){
}
SensitiveParameterValue Object

# var_export()
\SensitiveParameterValue::__set_state(array(
))

# (array) / json_encode()
array(0) {
}
string(2) "{}"

# ->getValue()
string(6) "secret"
