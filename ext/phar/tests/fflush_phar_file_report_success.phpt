--TEST--
fflush() on phar file should report success
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php

$phar = new Phar(__DIR__.'/fflush_phar_file_report_success.phar');
$phar->addFromString('test', 'contents');
unset($phar);

$f = fopen('phar://' . __DIR__.'/fflush_phar_file_report_success.phar/test', 'w');
var_dump(fflush($f));
var_dump(fclose($f));

?>
--CLEAN--
<?php
@unlink(__DIR__.'/fflush_phar_file_report_success.phar');
?>
--EXPECT--
bool(true)
bool(true)
