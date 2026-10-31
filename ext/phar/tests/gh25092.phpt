--TEST--
GH-25092 (Double free in Phar::copy() on link and mounted entries)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
function tar_symlink($name, $target) {
    $h  = str_pad($name, 100, "\0");
    $h .= "0000777\0";
    $h .= "0000000\0";
    $h .= "0000000\0";
    $h .= "00000000000\0";
    $h .= "00000000000\0";
    $h .= str_repeat(' ', 8);
    $h .= '2';
    $h .= str_pad($target, 100, "\0");
    $h .= "ustar\0" . "00";
    $h  = str_pad($h, 512, "\0");

    $sum = 0;
    for ($i = 0; $i < 512; $i++) {
        $sum += ord($h[$i]);
    }
    return substr_replace($h, sprintf("%06o\0 ", $sum), 148, 8);
}

$tar = __DIR__ . '/gh25092.tar';
file_put_contents($tar, tar_symlink('l', 'a') . str_repeat("\0", 1024));

$t = new PharData($tar);
$t->copy('l', 'l2');
unset($t);

$t = new PharData($tar);
var_dump(isset($t['l']), isset($t['l2']));
unset($t);

$phar = __DIR__ . '/gh25092.phar';
file_put_contents(__DIR__ . '/gh25092.txt', 'hello');

$p = new Phar($phar);
$p->addFromString('x.txt', 'x');
$p->setStub('<?php __HALT_COMPILER(); ?>');
unset($p);

$p = new Phar($phar);
Phar::mount('phar://' . $phar . '/m.txt', __DIR__ . '/gh25092.txt');
$p->copy('m.txt', 'c.txt');
var_dump(isset($p['c.txt']));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25092.tar');
@unlink(__DIR__ . '/gh25092.phar');
@unlink(__DIR__ . '/gh25092.txt');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
