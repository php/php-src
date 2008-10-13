<?php

$fname = dirname(__FILE__) . '/write15.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$p = new Phar(__FILE__);
var_dump(isset($p["copied"]));
$p->copy("test.txt","copied");
var_dump(isset($p["copied"]));
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?php __HALT"."_COMPILER();";
?>
