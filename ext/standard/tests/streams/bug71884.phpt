--TEST--
Bug #71884 (Null pointer deref (segfault) in stream_context_get_default)
--FILE--
<?php
$arr=array();
$arr[0]['A']=0;
try {
    stream_context_get_default($arr);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: Options should have the form ["wrappername"]["optionname"] = $value
