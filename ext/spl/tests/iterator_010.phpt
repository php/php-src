--TEST--
SPL: EmptyIterator
--FILE--
<?php

echo "===EmptyIterator===\n";

foreach(new LimitIterator(new EmptyIterator(), 0, 3) as $key => $val)
{
    echo "$key=>$val\n";
}

?>
--EXPECT--
===EmptyIterator===
