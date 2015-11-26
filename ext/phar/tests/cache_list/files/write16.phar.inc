<?php

$fname = dirname(__FILE__) . '/write16.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$p = new Phar(__FILE__);
var_dump(isset($p["test.txt"]));
unset($p["test.txt"]);
var_dump(isset($p["test.txt"]));
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?php __HALT"."_COMPILER();";
$phar['test2.txt'] = "<?php __HALT"."_COMPILER();";
?>
