--TEST--
Test get_class() function : usage variations  - ensure class name case is preserved.
--FILE--
<?php
class caseSensitivityTest {}
var_dump(get_class(new caseSensitivityTest));

echo "Done";
?>
--EXPECT--
string(19) "caseSensitivityTest"
Done
