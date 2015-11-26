--TEST--
SPL: DoublyLinkedList: iterators
--FILE--
<?php
$dll = new SplDoublyLinkedList();
$dll->push(2);
$dll->push(3);
$dll->push(4);

$dll2 = clone $dll;

// std iterator
foreach($dll as $k=>$v) {
    echo "$k=>$v\n";
    // inner iterator
    foreach($dll as $k2=>$v2) {
        echo "->$k2=>$v2\n";
    }
}

echo "# deleted\n";

foreach($dll as $k=>$v) {
    echo "$k=>$v\n";
    unset($dll);
}

echo "# while popping\n";

foreach($dll2 as $k=>$v) {
    echo "$k=>$v\n";
    echo "popped ".$dll2->pop()."\n";
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
0=>2
->0=>2
->1=>3
->2=>4
1=>3
->0=>2
->1=>3
->2=>4
2=>4
->0=>2
->1=>3
->2=>4
# deleted
0=>2
1=>3
2=>4
# while popping
0=>2
popped 4
1=>3
popped 3
===DONE===
