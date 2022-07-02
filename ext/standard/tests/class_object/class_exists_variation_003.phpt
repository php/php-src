--TEST--
Test class_exists() function : usage variations  - case sensitivity
--FILE--
<?php
class caseSensitivityTest {}
var_dump(class_exists('casesensitivitytest'));

echo "Done"
?>
--EXPECT--
bool(true)
Done
