--TEST--
Bug #40432 (strip_tags() fails with greater than in attribute), binary variant
--FILE--
<?php
echo strip_tags(b'<span title="test > all">this</span>') . "\n";
?>
--EXPECT--
this
