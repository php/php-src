--TEST--
GH-19701 (Serialize/deserialize loses some data)
--CREDITS--
cuchac
DanielEScherzer
--FILE--
<?php

class Item {
    public $children = [];
    public $parent = null;

    public function __sleep() {
        return ["children", "parent"];
    }
}

$baseProduct = new Item();

$child = new Item();
$child->parent = $baseProduct;
$baseProduct->children = [ $child ];

$data = [clone $baseProduct, $baseProduct];

echo serialize($data), "\n";

?>
--EXPECTF--
a:2:{i:0;O:4:"Item":2:{s:8:"children";a:1:{i:0;O:4:"Item":2:{s:8:"children";a:0:{}s:6:"parent";O:4:"Item":2:{s:8:"children";a:1:{i:0;r:4;}s:6:"parent";N;}}}s:6:"parent";N;}i:1;r:6;}
