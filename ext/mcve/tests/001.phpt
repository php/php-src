--TEST--
Check for mcve presence
--SKIPIF--
<?php if (!extension_loaded("mcve")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
echo "mcve extension is available";
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
mcve extension is available
