--TEST--
Test method_exists() function : variation - Case sensitivity
--FILE--
<?php
echo "*** Testing method_exists() : variation ***\n";

Class caseSensitivityTest {
    public function myMethod() {}
}

var_dump(method_exists(new caseSensitivityTest, 'myMetHOD'));
var_dump(method_exists('casesensiTivitytest', 'myMetHOD'));

echo "Done";
?>
--EXPECT--
*** Testing method_exists() : variation ***
bool(false)
bool(false)
Done
