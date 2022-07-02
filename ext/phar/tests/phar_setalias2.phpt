--TEST--
Phar::setAlias() error
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = '<?php echo "first stub\n"; __HALT_COMPILER(); ?>';

$files = array();
$files['a'] = 'a';
$files['b'] = 'b';
$files['c'] = 'c';

include 'files/phar_test.inc';

$phar = new Phar($fname);
echo $phar->getAlias() . "\n";
$phar->setAlias('test');
echo $phar->getAlias() . "\n";
$b = $phar;
$phar = new Phar(__DIR__ . '/notphar.phar');
try {
    $phar->setAlias('test');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $b = new Phar(__DIR__ . '/nope.phar', 0, 'test');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.php');
?>
--EXPECTF--
hio
test
alias "test" is already used for archive "%sphar_setalias2.phar.php" and cannot be used for other archives
alias "test" is already used for archive "%sphar_setalias2.phar.php" cannot be overloaded with "%snope.phar"
