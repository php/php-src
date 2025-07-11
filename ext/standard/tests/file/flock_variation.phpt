--TEST--
Test flock() function: Variations
--FILE--
<?php
echo "*** Testing flock() fun with the various operation and
    wouldblock values                                ***\n";

$file = preg_replace("~\.phpt?$~", '', __FILE__);
$fp = fopen($file, "w");

/* array of operations */
$operations = [
  LOCK_SH,
  LOCK_EX,
  LOCK_SH|LOCK_NB,
  LOCK_EX|LOCK_NB,
  LOCK_SH|LOCK_EX,
  LOCK_UN,
  1,
  2,
];

$i = 0;
foreach($operations as $operation) {
  echo "--- Outer iteration $i ---\n";
  var_dump(flock($fp, $operation, $wouldblock));
  $i++;
}

fclose($fp);
@unlink($file);

echo "\n*** Done ***\n";
?>
--EXPECT--
*** Testing flock() fun with the various operation and
    wouldblock values                                ***
--- Outer iteration 0 ---
bool(true)
--- Outer iteration 1 ---
bool(true)
--- Outer iteration 2 ---
bool(true)
--- Outer iteration 3 ---
bool(true)
--- Outer iteration 4 ---
bool(true)
--- Outer iteration 5 ---
bool(true)
--- Outer iteration 6 ---
bool(true)
--- Outer iteration 7 ---
bool(true)

*** Done ***
