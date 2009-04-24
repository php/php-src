--TEST--
Bug #40637 (strip_tags() does not handle single quotes correctly), binary variant
--FILE--
<?php

$html = b'<span title="Bug \' Trigger">Text</span>';
var_dump(strip_tags($html));

echo "Done\n";
?>
--EXPECT--
unicode(4) "Text"
Done
