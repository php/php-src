--TEST--
Phar::setStub()/getStub() tar-based
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';

$phar = new Phar($fname);
$stub = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>' ."\r\n";
$phar->setStub($stub);
$phar->setAlias('hio');
$phar['a'] = 'a';
$phar->stopBuffering();

var_dump($phar->getStub());
var_dump($phar->getStub() == $stub);

$newstub = '<?php echo "second stub\n"; _x_HALT_COMPILER(); ?>';

try {
    $phar->setStub($newstub);
} catch(exception $e) {
    echo 'Exception: ' . $e->getMessage() . "\n";
}

var_dump($phar->getStub());
var_dump($phar->getStub() == $stub);
$phar->stopBuffering();
var_dump($phar->getStub());
var_dump($phar->getStub() == $stub);

?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
__HALT_COMPILER();
?>
--EXPECTF--
string(50) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>
"
bool(true)
Exception: illegal stub for tar-based phar "%sphar_stub_error.phar.tar"
string(50) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>
"
bool(true)
string(50) "<?php echo "first stub\n"; __HALT_COMPILER(); ?>
"
bool(true)
