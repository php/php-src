--TEST--
Bug #71884 (Null pointer deref (segfault) in stream_context_get_default)
--FILE--
<?php
$arr=array();
$arr[0]['A']=0;
stream_context_get_default($arr);
?>
--EXPECTF--
Warning: stream_context_get_default(): options should have the form ["wrappername"]["optionname"] = $value in %sbug71884.php on line %d
