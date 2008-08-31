<?php
@unlink(dirname(__FILE__) . '/frontcontroller13.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller13.phar');
$a['index.php'] = '<?php
var_dump("test");
include "oof/test.php";';
$a['oof/test.php'] = '<?php
var_dump("oof/test.php");
include "./hi.php";';
$a['oof/hi.php'] = '<?php
var_dump("hi");';
$a->setStub('<?php
Phar::webPhar();
__HALT_COMPILER();');