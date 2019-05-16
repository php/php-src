--TEST--
Bug #24658 (combo of typehint / reference causes crash)
--FILE--
<?php
class foo {}
function no_typehint($a) {
	var_dump($a);
}
function typehint(foo $a) {
	var_dump($a);
}
function no_typehint_ref(&$a) {
	var_dump($a);
}
function typehint_ref(foo &$a) {
	var_dump($a);
}
$v = new foo();
$a = array(new foo(), 1, 2);
no_typehint($v);
typehint($v);
no_typehint_ref($v);
typehint_ref($v);
echo "===no_typehint===\n";
array_walk($a, 'no_typehint');
echo "===no_typehint_ref===\n";
array_walk($a, 'no_typehint_ref');
echo "===typehint===\n";
array_walk($a, 'typehint');
echo "===typehint_ref===\n";
array_walk($a, 'typehint_ref');
?>
--EXPECTF--
object(foo)#%d (0) {
}
object(foo)#%d (0) {
}
object(foo)#%d (0) {
}
object(foo)#%d (0) {
}
===no_typehint===
object(foo)#%d (0) {
}
int(1)
int(2)
===no_typehint_ref===
object(foo)#%d (0) {
}
int(1)
int(2)
===typehint===
object(foo)#%d (0) {
}

Fatal error: Uncaught TypeError: Argument 1 passed to typehint() must be an instance of foo, int given in %s:%d
Stack trace:
#0 [internal function]: typehint(1, 1)
#1 %s(%d): array_walk(Array, 'typehint')
#2 {main}
  thrown in %s on line %d
