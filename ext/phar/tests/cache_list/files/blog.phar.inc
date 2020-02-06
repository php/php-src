<?php

$fname = dirname(__FILE__) . '/blog.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?php if(file_exists(dirname(__FILE__) . "/files/config.xml")) {
    Phar::mount("config.xml", dirname(__FILE__) . "/files/config.xml");
}
Phar::webPhar("blog", "index.php");
__HALT_COMPILER(); ?>');
$phar['index.php'] = '<?php if (!file_exists("config.xml")) {
    include "install.php";
    exit;
}
var_dump(file_get_contents("config.xml"));
?>';
$phar['install.php'] = '<?php echo "install\n"; ?>';

?>
