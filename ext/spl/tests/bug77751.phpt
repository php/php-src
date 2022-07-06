--TEST--
Bug #77751: Writing to SplFileObject in ob_start gives segfault
--FILE--
<?php

echo "No crash.\n";

$logfile = new SplTempFileObject();
ob_start(function ($buffer) use ($logfile) {
    $logfile->fwrite($buffer);
    $logfile->fflush();
    return "";
});
echo "hmm\n";

?>
--EXPECT--
No crash.
