--TEST--
Static variables in functions
--SKIPIF--
<?php if(version_compare(zend_version(), "2.0.0-dev", '>=')) echo "skip removed in Zend Engine 2\n"; ?>
--FILE--
<?php 
old_function blah (
  static $hey=0,$yo=0;

  echo "hey=".$hey++.", ",$yo--."\n";
);
    
blah();
blah();
blah();
if (isset($hey) || isset($yo)) {
  echo "Local variables became global :(\n";
}
--EXPECT--
hey=0, 0
hey=1, -1
hey=2, -2
