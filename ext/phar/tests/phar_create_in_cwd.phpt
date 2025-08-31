--TEST--
Phar: attempt to create a Phar with relative path
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
chdir(__DIR__);
try {
    $p = new Phar('phar_create_in_cwd.phar');
    $p['file1.txt'] = 'hi';
    var_dump(strlen($p->getStub()));
    $p->setStub("<?php
spl_autoload_register(function(\$class) {
    include 'phar://' . str_replace('_', '/', \$class);
});
Phar::mapPhar('phar_create_in_cwd.phar');
include 'phar://phar_create_in_cwd.phar/startup.php';
__HALT_COMPILER();
?>");
    var_dump($p->getStub());
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/phar_create_in_cwd.phar');
?>
--EXPECTF--
int(6641)
string(%d) "<?php
spl_autoload_register(function($class) {
    include 'phar://' . str_replace('_', '/', $class);
});
Phar::mapPhar('phar_create_in_cwd.phar');
include 'phar://phar_create_in_cwd.phar/startup.php';
__HALT_COMPILER(); ?>
"
