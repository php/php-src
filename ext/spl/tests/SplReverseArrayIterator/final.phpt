--TEST--
Spl\ReverseArrayIterator: is final
--FILE--
<?php

$it = new Spl\ReverseArrayIterator([]);

echo 'Is final? ', (new ReflectionClass($it))->isFinal() ? 'yes' : 'no', "\n";

eval('class OopsIterator extends Spl\ReverseArrayIterator {}');

?>
--EXPECTF--
Is final? yes

Fatal error: Class OopsIterator may not inherit from final class (Spl\ReverseArrayIterator) in %s(%d) : eval()'d code on line %d
