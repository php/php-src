--TEST--
Test flock() function: Error conditions
--CONFLICTS--
obscure_filename
--FILE--
<?php
/*
Description: PHP supports a portable way of locking complete files
  in an advisory way
*/

echo "*** Testing error conditions ***\n";

$file = preg_replace("~\.phpt?$~", '.tmp', __FILE__);
$fp = fopen($file, "w");

/* array of operations */
$operations = array(
  0,
  LOCK_NB,
  FALSE,
  array(1,2,3),
  array(),
  "string",
  "",
  "\0"
);

$i = 0;
foreach($operations as $operation) {
    echo "--- Iteration $i ---" . \PHP_EOL;
    try {
        var_dump(flock($fp, $operation));
    } catch (\TypeError|\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    $i++;
}


/* Invalid arguments */
$fp = fopen($file, "w");
fclose($fp);
try {
    var_dump(flock($fp, LOCK_SH|LOCK_NB));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
$file = __DIR__."/flock_error.tmp";
unlink($file);
?>
--EXPECT--
*** Testing error conditions ***
--- Iteration 0 ---
flock(): Argument #2 ($operation) must be one of LOCK_SH, LOCK_EX, or LOCK_UN
--- Iteration 1 ---
flock(): Argument #2 ($operation) must be one of LOCK_SH, LOCK_EX, or LOCK_UN
--- Iteration 2 ---
flock(): Argument #2 ($operation) must be one of LOCK_SH, LOCK_EX, or LOCK_UN
--- Iteration 3 ---
flock(): Argument #2 ($operation) must be of type int, array given
--- Iteration 4 ---
flock(): Argument #2 ($operation) must be of type int, array given
--- Iteration 5 ---
flock(): Argument #2 ($operation) must be of type int, string given
--- Iteration 6 ---
flock(): Argument #2 ($operation) must be of type int, string given
--- Iteration 7 ---
flock(): Argument #2 ($operation) must be of type int, string given
flock(): supplied resource is not a valid stream resource
