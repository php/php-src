<?php

$fname = dirname(__FILE__) . '/write25.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$p = new Phar(__FILE__);
var_dump(isset($p["mounted"]));
Phar::mount("mounted", "phar://" . __FILE__ . "/test.txt");
var_dump(isset($p["mounted"]));
echo $p["mounted"]->getContent(),"\nok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?php __HALT"."_COMPILER();";
?>
