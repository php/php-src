--TEST--
Bug #38322 (reading past array in sscanf() leads to segfault/arbitrary code execution)
--FILE--
<?php

$str = "a b c d e";
var_dump(sscanf("a ",'%1$s',$str));

echo "Done\n";
?>
--EXPECT--
int(1)
Done
