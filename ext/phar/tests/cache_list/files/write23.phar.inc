<?php

$fname = dirname(__FILE__) . '/write23.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$p = new Phar(__FILE__);
var_dump(isset($p["test.txt"]), isset($p["newname"]));
rename("phar://" . __FILE__ . "/test.txt", "phar://" . __FILE__ . "/newname");
var_dump(isset($p["test.txt"]), isset($p["newname"]));
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?php __HALT"."_COMPILER();";
?>
