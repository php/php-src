--TEST--
assert()
--POST--
--GET--
--FILE--
<?
assert_options(ASSERT_ACTIVE,1);
assert_options(ASSERT_CALLBACK,"a");
assert_options(ASSERT_QUIET_EVAL,1);
assert_options(ASSERT_WARNING,0);
assert_options(ASSERT_BAIL,1);

function a($file,$line,$myev)
{ echo "assertion failed $line,\"$myev\"\n";
}

$a = 0;
assert('$a != 0');
?>
--EXPECT--
assertion failed 13,"$a != 0"
