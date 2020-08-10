<?php
@unlink(dirname(__FILE__) . '/frontcontroller16.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller16.phar');
$a['index.php'] = '<?php
echo "hi";
';
$a->setStub('<?php
try {
Phar::webPhar("test.phar", "/index.php", null, array(), array("fail", "here"));
} catch (TypeError|Exception $e) {
die($e->getMessage() . "\n");
}
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
