--TEST--
Test is_subclass_of() function : usage variations  - case sensitivity
--FILE--
<?php
echo "*** Testing is_subclass_of() : usage variations ***\n";

echo "*** Testing is_a() : usage variations ***\n";

class caseSensitivityTest {}
class caseSensitivityTestChild extends caseSensitivityTest {}

var_dump(is_subclass_of('caseSensitivityTestCHILD', 'caseSensitivityTEST'));

echo "Done"
?>
--EXPECT--
*** Testing is_subclass_of() : usage variations ***
*** Testing is_a() : usage variations ***
bool(true)
Done
