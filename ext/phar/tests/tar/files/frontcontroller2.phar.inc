<?php
@unlink(dirname(__FILE__) . '/frontcontroller2.phar.tar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller2.phar.tar');
$a['a.php'] = 'hio';
$a['a.jpg'] = 'hio';
$a['a.phps'] = '<?php function hio(){}';
$a->setStub('<?php
Phar::webPhar("whatever", "a.php");
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
