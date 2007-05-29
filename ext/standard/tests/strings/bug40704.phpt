--TEST--
Bug #40704 (strip_tags() does not handle single quotes correctly)
--FILE--
<?php

$html = "<div>Bug ' Trigger</div> Missing Text";
var_dump(strip_tags($html));

echo "Done\n";
?>
--EXPECT--	
string(26) "Bug ' Trigger Missing Text"
Done
