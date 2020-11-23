--TEST--
Bug #71969 (str_replace returns an incorrect resulting array after a foreach by reference)
--FILE--
<?php
$a = array(
    array("one" => array("a"=>"0000", "b"=>"1111")),
);

//foreach by reference, changing the array value
foreach($a as &$record)
{
    $record["one"]["a"] = "2222";
}
var_dump(str_replace("2", "3", $a));
?>
--EXPECTF--
Warning: Array to string conversion in %s on line %d
array(1) {
  [0]=>
  string(5) "Array"
}
