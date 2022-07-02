--TEST--
Bug #71884 (Null pointer deref (segfault) in stream_context_get_default)
--FILE--
<?php
$arr=array();
$arr[0]['A']=0;
try {
    stream_context_get_default($arr);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
Options should have the form ["wrappername"]["optionname"] = $value
