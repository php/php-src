--TEST--
Static variables in functions
--FILE--
<?php
function blah()
{
  static $hey=0,$yo=0;

  echo "hey=".$hey++.", ",$yo--."\n";
}

blah();
blah();
blah();
if (isset($hey) || isset($yo)) {
  echo "Local variables became global :(\n";
}
?>
--EXPECT--
hey=0, 0
hey=1, -1
hey=2, -2
