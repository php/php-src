<?php
@unlink(dirname(__FILE__) . '/frontcontroller.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller.phar');
$a['a.php'] = 'hio';
$a['a.jpg'] = 'hio';
$a['a.phps'] = '<?php function hio(){}';
$a['index.php'] = 'here is my index';
$a->setStub('<?php
Phar::webPhar();
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
