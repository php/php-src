<?php

$fname = dirname(__FILE__) . '/write7.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$p = new Phar(__FILE__);
var_dump(file_exists("phar://" . __FILE__ . "/test.txt"));
$p->delete("test.txt");
clearstatcache();
var_dump(file_exists("phar://" . __FILE__ . "/test.txt"));
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "hi
";
?>
