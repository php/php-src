--TEST--
SPL: RecursiveArrayIterator bug 44615
--CREDITS--
Julien Pauli <doctorrock83@gmail.com>
#testfest phpcampparis 2008-06-07
--FILE--
<?php
$a = new stdClass();

$array = array(array('z',$a),array('q','s'));

$rai = new RecursiveArrayIterator($array,RecursiveArrayIterator::CHILD_ARRAYS_ONLY);

foreach (new RecursiveIteratorIterator($rai) as $t) {
    var_dump($t);
}
echo "Second:\n";
$rai = new RecursiveArrayIterator($array);
foreach (new RecursiveIteratorIterator($rai) as $t) {
    var_dump($t);
}
?>
--EXPECTF--
unicode(1) "z"
object(stdClass)#1 (0) {
}
unicode(1) "q"
unicode(1) "s"
Second:
unicode(1) "z"
unicode(1) "q"
unicode(1) "s"
