--TEST--
Bug #78569 (proc_open() may require extra quoting)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip this test is for Windows platforms only');
?>
--XFAIL--
Before PHP 8.0.0 we would need to add extra quotes.
--FILE--
<?php
// We are searching for the following line:
// FIND ME
$descriptorspec = array(['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']);
$cmd = sprintf('"findstr" "/b" "/c:// FIND ME" "%s" 2>&1', __FILE__);
$proc = proc_open($cmd, $descriptorspec, $pipes);
fpassthru($pipes[1]);
proc_close($proc);
?>
--EXPECT--
// FIND ME
