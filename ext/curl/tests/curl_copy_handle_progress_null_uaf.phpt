--TEST--
Cloning a curl handle whose CURLOPT_PROGRESSFUNCTION was reset to null must not reference the source handle
--EXTENSIONS--
curl
--FILE--
<?php
$srcfile = __DIR__ . '/curl_copy_handle_progress_null_uaf.txt';
file_put_contents($srcfile, "Test.");

$ch1 = curl_init();
curl_setopt($ch1, CURLOPT_URL, "file://" . $srcfile);
curl_setopt($ch1, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch1, CURLOPT_NOPROGRESS, false);
curl_setopt($ch1, CURLOPT_PROGRESSFUNCTION, fn() => 0);
curl_setopt($ch1, CURLOPT_PROGRESSFUNCTION, null);

$ch2 = clone $ch1;
unset($ch1);

var_dump(curl_exec($ch2));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/curl_copy_handle_progress_null_uaf.txt');
?>
--EXPECT--
string(5) "Test."
