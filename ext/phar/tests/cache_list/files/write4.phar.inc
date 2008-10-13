<?php

$fname = dirname(__FILE__) . '/write4.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
var_dump(file_exists("phar://" . __FILE__ . "/test.txt"));
clearstatcache();
Phar::mount("test.txt", "phar://" . __FILE__ . "/tobemounted");
var_dump(file_exists("phar://" . __FILE__ . "/test.txt"), file_get_contents("phar://" . __FILE__ . "/test.txt"));
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['tobemounted'] = "hi";
?>
