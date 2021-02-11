--TEST--
Spl\ForwardArrayIterator: Countable
--FILE--
<?php

$it = new Spl\ForwardArrayIterator([]);

echo 'Countable? ', ($it instanceof Countable ? 'yes' : 'no'), "\n";
echo 'Count: ', count($it), "\n";

$it = new Spl\ForwardArrayIterator([1, 3]);
echo 'Count: ', count($it), "\n";

?>
--EXPECTF--
Countable? yes
Count: 0
Count: 2

