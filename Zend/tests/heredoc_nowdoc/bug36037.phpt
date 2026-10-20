--TEST--
Bug #36037 (heredoc adds extra line number)
--FILE--
<?php
echo __LINE__, "\n";
$x=<<<XXX
123
YYY;
XXX;
echo __LINE__, "\n";
?>
--EXPECT--
2
7
