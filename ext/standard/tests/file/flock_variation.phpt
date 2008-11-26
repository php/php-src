--TEST--
Test flock() function: usage variations
--FILE--
<?php
/* 
Prototype: bool flock(resource $handle, int $operation [, int &$wouldblock]);
Description: PHP supports a portable way of locking complete files in an advisory way
*/

echo "*** Test flock() function: with the operations given as numeric values ***\n";

$filename = dirname(__FILE__)."/flock_variation.tmp";
$file_handle = fopen($filename, "w");

/* array of operations */
$operations = array(
  1,  //nothing but LOCK_SH
  2,  //nothing but LOCK_EX
  2.234,  //nothing but LOCK_EX
  TRUE  //nothing but LOCK_SH
);

$i = 0;
foreach($operations as $operation) {
  var_dump(flock($file_handle, $operation));
  var_dump(flock($file_handle, 3));  //nothing but LOCK_UN
  $i++;
}

fclose($file_handle);
unlink($filename);

echo "*** Done ***\n";
?>
--EXPECTF--	
*** Test flock() function: with the operations given as numeric values ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
*** Done ***
