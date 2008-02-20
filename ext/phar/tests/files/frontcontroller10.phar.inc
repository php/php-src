<?php
@unlink(dirname(__FILE__) . '/frontcontroller10.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller10.phar');
$a['index.php'] = '<?php
var_dump($_SERVER["PHP_SELF"]);
var_dump($_SERVER["SCRIPT_NAME"]);
var_dump($_SERVER["SCRIPT_FILENAME"]);
var_dump($_SERVER["REQUEST_URI"]);
var_dump($_SERVER["PHAR_PHP_SELF"]);
var_dump($_SERVER["PHAR_SCRIPT_NAME"]);
var_dump($_SERVER["PHAR_SCRIPT_FILENAME"]);
var_dump($_SERVER["PHAR_REQUEST_URI"]);
';
$a->setStub('<?php
Phar::mungServer(array("PHP_SELF", "SCRIPT_NAME", "SCRIPT_FILENAME", "REQUEST_URI", "OOPSIE"));
Phar::webPhar();
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
