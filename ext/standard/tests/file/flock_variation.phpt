--TEST--
Test flock() function: Variations
--FILE--
<?php
echo "*** Testing flock() fun with the various operation and
    wouldblock values                                ***\n";

$file = preg_replace("~\.phpt?$~", '', __FILE__);
$fp = fopen($file, "w");

/* array of operations */
$operations = array(
  LOCK_SH,
  LOCK_EX,
  LOCK_SH|LOCK_NB,
  LOCK_EX|LOCK_NB,
  LOCK_SH|LOCK_EX,
  LOCK_UN,
  1,
  2,
  TRUE
);

/* array of wouldblocks */
$wouldblocks = array(
  0,
  1,
  2,
  1.234,
  TRUE,
  FALSE,
  NULL,
  array(1,2,3),
  array(),
  "string",
  "",
  /* binary input */
  b"string",
  b"",
  "\0"
);

$i = 0;
foreach($operations as $operation) {
  echo "--- Outer iteration $i ---\n";
  var_dump(flock($fp, $operation));
  $j = 0;
  foreach($wouldblocks as $wouldblock) {
    echo "-- Inner iteration $j in $i --\n";
    var_dump(flock($fp, $operation, $wouldblock));
    $j++;
  }
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
-- Inner iteration 0 in 0 --
bool(true)
-- Inner iteration 1 in 0 --
bool(true)
-- Inner iteration 2 in 0 --
bool(true)
-- Inner iteration 3 in 0 --
bool(true)
-- Inner iteration 4 in 0 --
bool(true)
-- Inner iteration 5 in 0 --
bool(true)
-- Inner iteration 6 in 0 --
bool(true)
-- Inner iteration 7 in 0 --
bool(true)
-- Inner iteration 8 in 0 --
bool(true)
-- Inner iteration 9 in 0 --
bool(true)
-- Inner iteration 10 in 0 --
bool(true)
-- Inner iteration 11 in 0 --
bool(true)
-- Inner iteration 12 in 0 --
bool(true)
-- Inner iteration 13 in 0 --
bool(true)
--- Outer iteration 1 ---
bool(true)
-- Inner iteration 0 in 1 --
bool(true)
-- Inner iteration 1 in 1 --
bool(true)
-- Inner iteration 2 in 1 --
bool(true)
-- Inner iteration 3 in 1 --
bool(true)
-- Inner iteration 4 in 1 --
bool(true)
-- Inner iteration 5 in 1 --
bool(true)
-- Inner iteration 6 in 1 --
bool(true)
-- Inner iteration 7 in 1 --
bool(true)
-- Inner iteration 8 in 1 --
bool(true)
-- Inner iteration 9 in 1 --
bool(true)
-- Inner iteration 10 in 1 --
bool(true)
-- Inner iteration 11 in 1 --
bool(true)
-- Inner iteration 12 in 1 --
bool(true)
-- Inner iteration 13 in 1 --
bool(true)
--- Outer iteration 2 ---
bool(true)
-- Inner iteration 0 in 2 --
bool(true)
-- Inner iteration 1 in 2 --
bool(true)
-- Inner iteration 2 in 2 --
bool(true)
-- Inner iteration 3 in 2 --
bool(true)
-- Inner iteration 4 in 2 --
bool(true)
-- Inner iteration 5 in 2 --
bool(true)
-- Inner iteration 6 in 2 --
bool(true)
-- Inner iteration 7 in 2 --
bool(true)
-- Inner iteration 8 in 2 --
bool(true)
-- Inner iteration 9 in 2 --
bool(true)
-- Inner iteration 10 in 2 --
bool(true)
-- Inner iteration 11 in 2 --
bool(true)
-- Inner iteration 12 in 2 --
bool(true)
-- Inner iteration 13 in 2 --
bool(true)
--- Outer iteration 3 ---
bool(true)
-- Inner iteration 0 in 3 --
bool(true)
-- Inner iteration 1 in 3 --
bool(true)
-- Inner iteration 2 in 3 --
bool(true)
-- Inner iteration 3 in 3 --
bool(true)
-- Inner iteration 4 in 3 --
bool(true)
-- Inner iteration 5 in 3 --
bool(true)
-- Inner iteration 6 in 3 --
bool(true)
-- Inner iteration 7 in 3 --
bool(true)
-- Inner iteration 8 in 3 --
bool(true)
-- Inner iteration 9 in 3 --
bool(true)
-- Inner iteration 10 in 3 --
bool(true)
-- Inner iteration 11 in 3 --
bool(true)
-- Inner iteration 12 in 3 --
bool(true)
-- Inner iteration 13 in 3 --
bool(true)
--- Outer iteration 4 ---
bool(true)
-- Inner iteration 0 in 4 --
bool(true)
-- Inner iteration 1 in 4 --
bool(true)
-- Inner iteration 2 in 4 --
bool(true)
-- Inner iteration 3 in 4 --
bool(true)
-- Inner iteration 4 in 4 --
bool(true)
-- Inner iteration 5 in 4 --
bool(true)
-- Inner iteration 6 in 4 --
bool(true)
-- Inner iteration 7 in 4 --
bool(true)
-- Inner iteration 8 in 4 --
bool(true)
-- Inner iteration 9 in 4 --
bool(true)
-- Inner iteration 10 in 4 --
bool(true)
-- Inner iteration 11 in 4 --
bool(true)
-- Inner iteration 12 in 4 --
bool(true)
-- Inner iteration 13 in 4 --
bool(true)
--- Outer iteration 5 ---
bool(true)
-- Inner iteration 0 in 5 --
bool(true)
-- Inner iteration 1 in 5 --
bool(true)
-- Inner iteration 2 in 5 --
bool(true)
-- Inner iteration 3 in 5 --
bool(true)
-- Inner iteration 4 in 5 --
bool(true)
-- Inner iteration 5 in 5 --
bool(true)
-- Inner iteration 6 in 5 --
bool(true)
-- Inner iteration 7 in 5 --
bool(true)
-- Inner iteration 8 in 5 --
bool(true)
-- Inner iteration 9 in 5 --
bool(true)
-- Inner iteration 10 in 5 --
bool(true)
-- Inner iteration 11 in 5 --
bool(true)
-- Inner iteration 12 in 5 --
bool(true)
-- Inner iteration 13 in 5 --
bool(true)
--- Outer iteration 6 ---
bool(true)
-- Inner iteration 0 in 6 --
bool(true)
-- Inner iteration 1 in 6 --
bool(true)
-- Inner iteration 2 in 6 --
bool(true)
-- Inner iteration 3 in 6 --
bool(true)
-- Inner iteration 4 in 6 --
bool(true)
-- Inner iteration 5 in 6 --
bool(true)
-- Inner iteration 6 in 6 --
bool(true)
-- Inner iteration 7 in 6 --
bool(true)
-- Inner iteration 8 in 6 --
bool(true)
-- Inner iteration 9 in 6 --
bool(true)
-- Inner iteration 10 in 6 --
bool(true)
-- Inner iteration 11 in 6 --
bool(true)
-- Inner iteration 12 in 6 --
bool(true)
-- Inner iteration 13 in 6 --
bool(true)
--- Outer iteration 7 ---
bool(true)
-- Inner iteration 0 in 7 --
bool(true)
-- Inner iteration 1 in 7 --
bool(true)
-- Inner iteration 2 in 7 --
bool(true)
-- Inner iteration 3 in 7 --
bool(true)
-- Inner iteration 4 in 7 --
bool(true)
-- Inner iteration 5 in 7 --
bool(true)
-- Inner iteration 6 in 7 --
bool(true)
-- Inner iteration 7 in 7 --
bool(true)
-- Inner iteration 8 in 7 --
bool(true)
-- Inner iteration 9 in 7 --
bool(true)
-- Inner iteration 10 in 7 --
bool(true)
-- Inner iteration 11 in 7 --
bool(true)
-- Inner iteration 12 in 7 --
bool(true)
-- Inner iteration 13 in 7 --
bool(true)
--- Outer iteration 8 ---
bool(true)
-- Inner iteration 0 in 8 --
bool(true)
-- Inner iteration 1 in 8 --
bool(true)
-- Inner iteration 2 in 8 --
bool(true)
-- Inner iteration 3 in 8 --
bool(true)
-- Inner iteration 4 in 8 --
bool(true)
-- Inner iteration 5 in 8 --
bool(true)
-- Inner iteration 6 in 8 --
bool(true)
-- Inner iteration 7 in 8 --
bool(true)
-- Inner iteration 8 in 8 --
bool(true)
-- Inner iteration 9 in 8 --
bool(true)
-- Inner iteration 10 in 8 --
bool(true)
-- Inner iteration 11 in 8 --
bool(true)
-- Inner iteration 12 in 8 --
bool(true)
-- Inner iteration 13 in 8 --
bool(true)

*** Done ***
