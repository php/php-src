--TEST--
Bug #42119 (array_push($arr,&$obj) doesn't work with zend.ze1_compatibility_mode On)
--INI--
allow_call_time_pass_reference=1
zend.ze1_compatibility_mode=1
--FILE--
<?php
class myclass {
  var $item = 1;
}

$arr = array();
@$myobj = new myclass();
array_push($arr,&$myobj);
$myobj->item = 2;
echo $arr[0]->item,"\n";
?>
--EXPECT--
2
