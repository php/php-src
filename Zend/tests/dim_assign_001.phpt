--TEST--
JIT - Assigning to arrays using string key which parses to an integer
--FILE--
<?php
/* We are going to store a value in an array, using the key "1"
 * PHP should always convert such strings to integers when storing or retrieving
 * values from an array
 *
 * We'll do it in a loop, so that if JIT is enabled, the code will be JIT'd
 * (Because this test was originally added as a regression test for a JIT bug)
 *
 * Also, the test will do this in a way which guarantees PHP won't be able to
 * predict whether the (string) key will be a numeric string or not */
$fp = fopen(realpath(__DIR__ . '/dim_assign_001.txt'), 'r+');
$array = array();
while ($line = fgets($fp, 256)) {
  sscanf($line, '%x', $char);
  $char = chr($char);
  $array[$char] = "Values can be stored correctly using numeric string keys";
}
var_dump($array['1']);
?>
--EXPECT--
string(56) "Values can be stored correctly using numeric string keys"
