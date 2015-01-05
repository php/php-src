<?php
@unlink(dirname(__FILE__) . '/frontcontroller19.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller19.phar');
$a['start/index.php'] = '<?php
echo "start/index.php\n";
include "./another.php";
';
$a['start/another.php'] = '<?php
echo "start/another.php\n";
include "../another.php";
?>';
$a['another.php'] = '<?php
echo "another.php\n";
?>';
$a->setStub('<?php
set_include_path("phar://" . __FILE__);
try {
Phar::webPhar("test.phar", "/start/index.php");
} catch (Exception $e) {
die($e->getMessage() . "\n");
}
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
