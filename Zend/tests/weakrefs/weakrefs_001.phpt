--TEST--
WeakRef
--FILE--
<?php
$std = new stdClass;

debug_zval_dump($std);

$wr = new WeakRef($std);
$wr2 = new WeakRef($std);
$wr3 = clone $wr2;

debug_zval_dump($std);

var_dump($wr, $wr2, $wr3);

debug_zval_dump($wr->get());
debug_zval_dump($wr2->get());
debug_zval_dump($wr3->get());

unset($std);

debug_zval_dump($wr->get());
debug_zval_dump($wr2->get());
debug_zval_dump($wr3->get());
?>
--EXPECT--
object(stdClass)#1 (0) refcount(2){
}
object(stdClass)#1 (0) refcount(2){
}
object(WeakRef)#2 (0) {
}
object(WeakRef)#3 (0) {
}
object(WeakRef)#4 (0) {
}
object(stdClass)#1 (0) refcount(2){
}
object(stdClass)#1 (0) refcount(2){
}
object(stdClass)#1 (0) refcount(2){
}
NULL
NULL
NULL

