--TEST--
TokenList: iteration 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C D E F"/>');
$list = $dom->documentElement->classList;

foreach ($list as $i => $item) {
    var_dump($i, $item);
    $list->remove('A', 'D');
}

?>
--EXPECT--
int(0)
string(1) "A"
int(0)
string(1) "B"
int(1)
string(1) "C"
int(2)
string(1) "E"
int(3)
string(1) "F"
