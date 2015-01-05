--TEST--
Bug #40432 (strip_tags() fails with greater than in attribute)
--FILE--
<?php
echo strip_tags('<span title="test > all">this</span>') . "\n";
?>
--EXPECT--
this
