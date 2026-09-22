--TEST--
GH-17223 (Memory leak in libxml encoding handling)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument("1.0", "Shift-JIS");
@$doc->save("%00");
echo "Done\n";
?>
--EXPECT--
Done
