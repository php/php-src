--TEST--
Bug #71020 (Use after free in Collator::sortWithSortKeys)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

$var_3=new Collator("Whatever");
for($x=0;$x<0xbb;$x++)
    $myarray[substr(md5(microtime()),rand(0,26),9) . strval($x)]= substr(md5(microtime()),rand(0,26),9) . strval($x);
$var_3->sortWithSortKeys($myarray);
?>
okey
--EXPECT--
okey
