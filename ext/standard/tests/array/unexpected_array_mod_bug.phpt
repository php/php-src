--TEST--
Crash when function parameter modified via reference
--FILE--
<?php
function usercompare($a,$b) {
  unset($GLOBALS['my_var'][2]); 
  return 0;
}
$my_var = array(1 => "entry_1",
2 => "entry_2",
3 => "entry_3",
4 => "entry_4",
5 => "entry_5");
usort($my_var, "usercompare");

echo "Done.\n";
?>
--EXPECTF--

Warning: usort(): Array was modified by the user comparison function in %s on line %d
Done.
