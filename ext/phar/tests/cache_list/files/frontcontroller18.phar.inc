<?php
@unlink(dirname(__FILE__) . '/frontcontroller18.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller18.phar');
$a['index.php'] = '<?php
echo "hi";
';
$a->setStub('<?php
function s($a)
{
}
try {
Phar::webPhar("test.phar", "/index.php", null, array(), "s");
} catch (Exception $e) {
die($e->getMessage() . "\n");
}
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
