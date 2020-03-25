--TEST--
Bug #40637 (strip_tags() does not handle single quotes correctly)
--FILE--
<?php

$html = '<span title="Bug \' Trigger">Text</span>';
var_dump(strip_tags($html));

echo "Done\n";
?>
--EXPECT--
string(4) "Text"
Done
