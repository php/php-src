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
array_walk($a, 'no_typehint');
array_walk($a, 'no_typehint_ref');
array_walk($a, 'typehint');
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
object(foo)#%d (0) {
}
int(1)
int(2)
object(foo)#%d (0) {
}
int(1)
int(2)

Fatal error: Argument 1 must be an instance of foo in %s on line %d
