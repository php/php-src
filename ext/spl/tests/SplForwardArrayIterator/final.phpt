--TEST--
Spl\ForwardArrayIterator: is final
--FILE--
<?php

$it = new Spl\ForwardArrayIterator([]);

echo 'Is final? ', (new ReflectionClass($it))->isFinal() ? 'yes' : 'no', "\n";

eval('class OopsIterator extends Spl\ForwardArrayIterator {}');

?>
--EXPECTF--
Is final? yes

Fatal error: Class OopsIterator may not inherit from final class (Spl\ForwardArrayIterator) in %s(%d) : eval()'d code on line %d
