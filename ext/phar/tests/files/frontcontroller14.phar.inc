<?php
@unlink(dirname(__FILE__) . '/frontcontroller14.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller14.phar');
$a['html/index.php'] = '<?php
var_dump($_SERVER[b"PATH_INFO"]);
var_dump($_SERVER[b"PATH_TRANSLATED"]);
';
$a->setStub('<?php
function s($a)
{
    return "/html/index.php";
}
Phar::interceptFileFuncs();
Phar::mungServer(array("PHP_SELF", "REQUEST_URI"));
Phar::webPhar("whatever", "/html/index.php", null, array(), "s");
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
