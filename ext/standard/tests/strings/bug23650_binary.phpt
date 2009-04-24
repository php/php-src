--TEST--
Bug #23650 (strip_tags() removes hyphens), binary variant
--FILE--
<?php
$str = (binary) <<< HERE
1:<!-- abc -  -->
2:<!doctype -- >
3:
4:<abc - def>
5:abc - def
6:</abc>

HERE;

echo strip_tags($str);
echo strip_tags($str, b'<abc>');
?>
--EXPECT--
1:
2:
3:
4:
5:abc - def
6:
1:
2:
3:
4:<abc - def>
5:abc - def
6:</abc>
