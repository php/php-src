--TEST--
See if the satellite extension is loaded
--SKIPIF--
<?php if (!extension_loaded("satellite")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
echo "satellite extension is available";
/*
	you can add regression tests for your extension here

  the output of your test code has to be equal to the
  text in the --EXPECT-- section below for the tests
  to pass, differences between the output and the
  expected text are interpreted as failure

	see php4/tests/README for further information on
  writing regression tests
*/
?>
--EXPECT--
satellite extension is available
