<?php

$fname = dirname(__FILE__) . '/write.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php
$phar = new Phar(__FILE__);
echo $phar["test.txt"]->getContent();
$phar["test.txt"] = "changed
";
echo $phar["test.txt"]->getContent();
echo "ok\n";
__HALT_COMPILER();
?>');
$phar->setMetadata('hi');
$phar['test.txt'] = "hi
";
$phar['test.txt']->setMetadata('hi');
?>
