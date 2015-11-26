<?php
@unlink(dirname(__FILE__) . '/frontcontroller8.phar.tar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller8.phar.tar');
$a['a.phps'] = 'hio1';
$a['a.jpg'] = 'hio2';
$a['a.php'] = '<?php function hio(){}';
$a['fronk.gronk'] = 'hio3';
$a->setStub('<?php
Phar::webPhar("whatever", "index.php", null, array("jpg" => "foo/bar", "phps" => Phar::PHP, "php" => Phar::PHPS));
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
