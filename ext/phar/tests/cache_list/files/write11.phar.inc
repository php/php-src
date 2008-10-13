<?php

$fname = dirname(__FILE__) . '/write11.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$p = new Phar(__FILE__);
var_dump($p->getStub());
$p2 = new Phar(__FILE__);
$p->setDefaultStub();
echo strlen($p2->getStub()),"\n";
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?php __HALT"."_COMPILER();";
?>
