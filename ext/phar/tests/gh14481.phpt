--TEST--
GH-14481 (realpath() and SplFileInfo::getRealPath inside Phar)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$file = __DIR__ . DIRECTORY_SEPARATOR . 'gh14481.phar';
@unlink($file);

$phar = new Phar($file);
$phar->addFromString('inner.php', "<?php echo 'hi';");
$phar->setStub("<?php __HALT_COMPILER();");
unset($phar);

$existing = "phar://" . $file . "/inner.php";
$missing  = "phar://" . $file . "/nope.php";

echo "realpath existing entry:\n";
var_dump(realpath($existing));

echo "realpath missing entry:\n";
var_dump(realpath($missing));

echo "SplFileInfo existing entry:\n";
var_dump((new SplFileInfo($existing))->getRealPath());

echo "SplFileInfo missing entry:\n";
var_dump((new SplFileInfo($missing))->getRealPath());

echo "PharFileInfo existing entry:\n";
var_dump((new PharFileInfo($existing))->getRealPath());

echo "RecursiveIteratorIterator entry:\n";
foreach (new RecursiveIteratorIterator(new Phar($file)) as $info) {
    var_dump($info->getRealPath());
}

echo "plain-filesystem branch unaffected:\n";
var_dump(realpath(__FILE__) === __FILE__);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh14481.phar');
?>
--EXPECTF--
realpath existing entry:
string(%d) "phar://%sgh14481.phar/inner.php"
realpath missing entry:
bool(false)
SplFileInfo existing entry:
string(%d) "phar://%sgh14481.phar/inner.php"
SplFileInfo missing entry:
bool(false)
PharFileInfo existing entry:
string(%d) "phar://%sgh14481.phar/inner.php"
RecursiveIteratorIterator entry:
string(%d) "phar://%sgh14481.phar/inner.php"
plain-filesystem branch unaffected:
bool(true)
