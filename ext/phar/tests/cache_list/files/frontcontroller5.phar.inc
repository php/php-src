<?php
@unlink(dirname(__FILE__) . '/frontcontroller5.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller5.phar');
$a['a.php'] = 'hio';
$a['a.jpg'] = 'hio';
$a['a.phps'] = '<?php function hio(){}';
$a->setStub('<?php
try {
	Phar::webPhar("whatever", "index.php", null, array(0 => "oops"));
} catch (Throwable $e) {
	die($e::class . ": " . $e->getMessage() . "\n");
}
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
