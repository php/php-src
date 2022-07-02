--TEST--
Test array_count_values() function : Test all normal parameter variations
--FILE--
<?php
/*
 * Test behaviour with parameter variations
 */

echo "*** Testing array_count_values() : parameter variations ***\n";

class A {
    static function hello() {
      echo "Hello\n";
    }
}

$ob = new A();

$fp = fopen("array_count_file", "w+");

$arrays = array ("bobk" => "bobv", "val", 6 => "val6",  $fp, $ob);

var_dump (@array_count_values ($arrays));
echo "\n";


echo "Done";
?>
--CLEAN--
<?php
unlink("array_count_file");
?>
--EXPECT--
*** Testing array_count_values() : parameter variations ***
array(3) {
  ["bobv"]=>
  int(1)
  ["val"]=>
  int(1)
  ["val6"]=>
  int(1)
}

Done
