<?php

$fname = dirname(__FILE__) . '/write3.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
clearstatcache();
var_dump(file_exists("phar://" . __FILE__ . "/test"), is_dir("phar://" . __FILE__ . "/test"));
rmdir("phar://" . __FILE__ . "/test");
clearstatcache();
var_dump(file_exists("phar://" . __FILE__ . "/test"), is_dir("phar://" . __FILE__ . "/test"));
echo "ok\n";
__HALT_COMPILER();
?>');
$phar->setMetadata('hi');
$phar['test.txt'] = "hi
";
$phar['test.txt']->setMetadata('hi');
$phar->addEmptyDir('test');
?>
