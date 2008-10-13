<?php

$fname = dirname(__FILE__) . '/write19.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$p = new Phar(__FILE__);
var_dump($p["test.txt"]->getMetadata());
$p["test.txt"]->setMetadata("hi2");
var_dump($p["test.txt"]->getMetadata());
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?php __HALT"."_COMPILER();";
$phar["test.txt"]->setMetadata("hi");
?>
