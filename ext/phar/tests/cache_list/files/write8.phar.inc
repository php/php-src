<?php

$fname = dirname(__FILE__) . '/write8.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$p = new Phar(__FILE__);
var_dump($p->getAlias());
$p2 = new Phar(__FILE__);
$p->setAlias("hi");
echo $p2->getAlias(),"\n";
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "hi
";
?>
