--TEST--
Check removing an item from an array when the offset is not an integer.
--CREDITS--
PHPNW Testfest 2009 - Paul Court ( g@rgoyle.com )
--FILE--
<?php
    // Create a fixed array
    $fixedArray = new SplFixedArray(5);

    // Fill it up
    for ($i=0; $i < 5; $i++) {
        $fixedArray[$i] = "PHPNW Testfest";
    }

    // remove an item
    $fixedArray->offsetUnset("4");

    var_dump($fixedArray);

?>
--EXPECT--
object(SplFixedArray)#1 (5) {
  [0]=>
  string(14) "PHPNW Testfest"
  [1]=>
  string(14) "PHPNW Testfest"
  [2]=>
  string(14) "PHPNW Testfest"
  [3]=>
  string(14) "PHPNW Testfest"
  [4]=>
  NULL
}
