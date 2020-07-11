--TEST--
Test is_a() function : usage variations  - case sensitivity
--FILE--
<?php
echo "*** Testing is_a() : usage variations ***\n";

class caseSensitivityTest {}
class caseSensitivityTestChild extends caseSensitivityTest {}

var_dump(is_a(new caseSensitivityTestChild, 'caseSensitivityTEST'));

echo "Done";
?>
--EXPECT--
*** Testing is_a() : usage variations ***
bool(true)
Done
