--TEST--
Bug #43495 (array_merge_recursive() crashes with recursive arrays)
--FILE--
<?php
$a=array("key1"=>array("key2"=>array()));
$a["key1"]["key2"]["key3"]=&$a;

$b=array("key1"=>array("key2"=>array()));
$b["key1"]["key2"]["key3"]=&$b;


try {
    array_merge_recursive($a,$b);
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

/* Break recursion */
$a["key1"]["key2"]["key3"] = null;
$b["key1"]["key2"]["key3"] = null;

?>
--EXPECT--
Recursion detected
