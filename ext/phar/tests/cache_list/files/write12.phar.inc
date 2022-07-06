<?php

$fname = dirname(__FILE__) . '/write12.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$p = new Phar(__FILE__);
var_dump($p->getSignature());
$p2 = new Phar(__FILE__);
$p->setSignatureAlgorithm(Phar::MD5);
var_dump($p->getSignature());
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?php __HALT"."_COMPILER();";
?>
