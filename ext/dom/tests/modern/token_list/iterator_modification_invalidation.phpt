--TEST--
Test DOMTokenList iterator invalidation after modification
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C D"/>');
$list = $dom->documentElement->classList;


echo "---\n";

$counter = 0;
foreach ($list as $key => $token) {
    var_dump($key, $token);
    if (++$counter === 2) {
        $list->value = 'E F G';
    }
}

echo "---\n";

$iterator = $list->getIterator();
$iterator->next();
$list->value = 'X Y Z';
var_dump($iterator->key());
var_dump($iterator->current());
$iterator->rewind();
var_dump($iterator->key());
var_dump($iterator->current());
$list->value = '';
var_dump($iterator->key());
var_dump($iterator->current());
var_dump($iterator->valid());

?>
--EXPECT--
---
int(0)
string(1) "A"
int(1)
string(1) "B"
int(2)
string(1) "G"
---
int(1)
string(1) "Y"
int(0)
string(1) "X"
int(0)
NULL
bool(false)
