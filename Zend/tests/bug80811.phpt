--TEST--
Bug #80811: Function exec without $output but with $restult_code parameter crashes
--FILE--
<?php

echo 'Executing with all params:' . PHP_EOL;
exec('echo Something', output: $output, result_code: $resultCode);
var_dump($resultCode);

echo 'Executing without output param:' . PHP_EOL;
exec('echo Something', result_code: $resultCode);
var_dump($resultCode);

?>
--EXPECT--
Executing with all params:
int(0)
Executing without output param:
int(0)
