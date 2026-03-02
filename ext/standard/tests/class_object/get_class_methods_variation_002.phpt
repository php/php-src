--TEST--
Test get_class_methods() function : usage variations  - case sensitivity
--FILE--
<?php
echo "*** Testing get_class_methods() : usage variations ***\n";

class caseSensitivityTest {
    function MyMeThOd() {}
}

var_dump( get_class_methods('CasesensitivitytesT') );

echo "Done";
?>
--EXPECT--
*** Testing get_class_methods() : usage variations ***
array(1) {
  [0]=>
  string(8) "MyMeThOd"
}
Done
