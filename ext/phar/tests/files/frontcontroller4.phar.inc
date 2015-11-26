<?php
@unlink(dirname(__FILE__) . '/frontcontroller4.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller4.phar');
$a['a.php'] = 'hio';
$a['a.jpg'] = 'hio';
$a['a.phps'] = '<?php function hio(){}';
$a->setStub('<?php
function s($a)
{
    static $b = array(b"/hi" => false);
    if (isset($b[$a])) return $b[$a];
    return $a;
}
Phar::webPhar("whatever", "index.php", null, array(), "s");
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
