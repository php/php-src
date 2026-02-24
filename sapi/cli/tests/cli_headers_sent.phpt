--TEST--
CLI headers_sent()
--SKIPIF--
<?php include "skipif.inc" ?>
--FILE--
<?php
$sent1 = headers_sent();

fwrite(STDOUT, "Hi !!");
$sent2 = headers_sent();

ob_start();
echo "Bye !!\n";
$sent3 = headers_sent();

$out = ob_get_clean();
$sent4 = headers_sent();

echo $out;

var_dump(
    $sent1,
    $sent2,
    $sent3,
    $sent4,
    headers_sent()
);
?>
--EXPECT--
Hi !!Bye !!
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
