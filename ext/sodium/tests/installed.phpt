--TEST--
Check for sodium presence
--EXTENSIONS--
sodium
--FILE--
<?php
echo "sodium extension is available";
/*
        you can add regression tests for your extension here

  the output of your test code has to be equal to the
  text in the--EXPECT-- section below for the tests
  to pass, differences between the output and the
  expected text are interpreted as failure
*/
?>
--EXPECT--
sodium extension is available
