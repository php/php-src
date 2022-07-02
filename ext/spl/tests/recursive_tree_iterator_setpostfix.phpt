--TEST--
SPL: RecursiveTreeIterator::setPostfix()
--CREDITS--
Joshua Thijssen (jthijssen@noxlogic.nl)
--FILE--
<?php

$arr = array(
    0 => array(
        "a",
        1,
    ),
    "a" => array(
        2,
        "b",
        3 => array(
            4,
            "c",
        ),
        "3" => array(
            4,
            "c",
        ),
    ),
);

$it = new RecursiveArrayIterator($arr);
$it = new RecursiveTreeIterator($it);

echo "----\n";
echo $it->getPostfix();
echo "\n\n";

echo "----\n";
$it->setPostfix("POSTFIX");
echo $it->getPostfix();
echo "\n\n";

echo "----\n";
foreach($it as $k => $v) {
    echo "[$k] => $v\n";
}

echo "----\n";
$it->setPostfix("");
echo $it->getPostfix();
echo "\n\n";

echo "----\n";
foreach($it as $k => $v) {
    echo "[$k] => $v\n";
}



?>
--EXPECT--
----


----
POSTFIX

----
[0] => |-ArrayPOSTFIX
[0] => | |-aPOSTFIX
[1] => | \-1POSTFIX
[a] => \-ArrayPOSTFIX
[0] =>   |-2POSTFIX
[1] =>   |-bPOSTFIX
[3] =>   \-ArrayPOSTFIX
[0] =>     |-4POSTFIX
[1] =>     \-cPOSTFIX
----


----
[0] => |-Array
[0] => | |-a
[1] => | \-1
[a] => \-Array
[0] =>   |-2
[1] =>   |-b
[3] =>   \-Array
[0] =>     |-4
[1] =>     \-c
