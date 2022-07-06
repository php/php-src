<?php

$fname = dirname(__FILE__) . '/write5.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$phar = new Phar(__FILE__);
$d = dirname(__FILE__) . "/copyonwrite5";
mkdir($d);
file_put_contents($d . "/file1", "file1\n");
file_put_contents($d . "/file2", "file2\n");
$arr = $phar->buildFromDirectory($d);
ksort($arr);
var_dump($arr);
$phar2 = new Phar(__FILE__);
$arr = array();
foreach ($phar2 as $name => $file) {
    $arr[$name] = $file->getContent();
}
ksort($arr);
foreach ($arr as $name => $content) {
    echo $name, " ", $content;
}
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['hi'] = "hi\n";
?>
