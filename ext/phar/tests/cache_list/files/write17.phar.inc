<?php

$fname = dirname(__FILE__) . '/write17.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$p = new Phar(__FILE__);
var_dump($p->getMetadata());
$p->setMetadata("hi");
var_dump($p->getMetadata());
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?php __HALT"."_COMPILER();";
?>
