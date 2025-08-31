--TEST--
Test get_parent_class() function : variation - case sensitivity
--FILE--
<?php
//  Note: basic use cases in Zend/tests/010.phpt

echo "*** Testing get_parent_class() : variation ***\n";

class caseSensitivityTest {}
class caseSensitivityTestChild extends caseSensitivityTest {}

var_dump(get_parent_class('CasesensitivitytestCHILD'));
var_dump(get_parent_class(new CasesensitivitytestCHILD));

echo "Done";
?>
--EXPECT--
*** Testing get_parent_class() : variation ***
string(19) "caseSensitivityTest"
string(19) "caseSensitivityTest"
Done
