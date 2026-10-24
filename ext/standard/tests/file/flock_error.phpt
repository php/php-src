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
fclose($fp);
try {
    var_dump(flock($fp, LOCK_SH|LOCK_NB));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
$file = __DIR__."/flock_error.tmp";
unlink($file);
?>
--EXPECT--
*** Testing error conditions ***
TypeError: flock(): Argument #1 ($stream) must be an open stream resource
