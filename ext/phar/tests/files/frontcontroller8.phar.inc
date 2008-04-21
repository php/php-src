<?php
@unlink(dirname(__FILE__) . '/frontcontroller8.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller8.phar');
$a['a.phps'] = 'hio1';
$a['a.jpg'] = 'hio2';
$a['a.php'] = '<?php function hio(){}';
$a['fronk.gronk'] = 'hio3';
$a['404.php'] = 'My 404 is rawesome';
$a->setStub('<?php
Phar::webPhar("whatever", "index.php", "404.php", array("jpg" => "foo/bar", "phps" => Phar::PHP, "php" => Phar::PHPS));
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
