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
$operations = [
  0,
  LOCK_NB,
];

foreach($operations as $operation) {
    try {
        var_dump(flock($fp, $operation));
    } catch (\TypeError|\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
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
flock(): Argument #2 ($operation) must be one of LOCK_SH, LOCK_EX, or LOCK_UN
flock(): Argument #2 ($operation) must be one of LOCK_SH, LOCK_EX, or LOCK_UN
flock(): Argument #1 ($stream) must be an open stream resource
