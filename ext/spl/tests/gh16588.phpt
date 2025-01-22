--TEST--
GH-16588 (UAF in Observer->serialize)
--CREDITS--
chibinz
--FILE--
<?php

class C {
    function __serialize(): array {
        global $store;
        $store->removeAll($store);
        return [];
    }
}

$store = new SplObjectStorage;
$store[new C] = new stdClass;
var_dump($store->serialize());

?>
--EXPECT--
string(47) "x:i:1;O:1:"C":0:{},O:8:"stdClass":0:{};m:a:0:{}"
