<?php
@unlink(dirname(__FILE__) . '/frontcontroller3.phar.zip');
$a = new Phar(dirname(__FILE__) . '/frontcontroller3.phar.zip');
$a['a.php'] = 'hio';
$a['a.jpg'] = 'hio';
$a['a.phps'] = '<?php function hio(){}';
$a->setStub('<?php
function s($a)
{
    static $b = array(b"/hi" => b"a.phps");
    if (isset($b[$a])) return $b[$a];
    return $a;
}
Phar::webPhar("whatever", "/index.php", null, array(), "s");
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
