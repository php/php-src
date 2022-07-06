--TEST--
Test trait_exists() function : usage variations  - case sensitivity
--FILE--
<?php
trait caseSensitivityTest {}
var_dump(trait_exists('casesensitivitytest'));

echo "Done"
?>
--EXPECT--
bool(true)
Done
