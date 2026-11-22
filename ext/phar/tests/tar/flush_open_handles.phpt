--TEST--
Phar: writing to an archive with open file handles tar-based
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.tar';

$phar = new PharData($fname);
$phar['a'] = 'aaaa';
$phar['b'] = 'bbbb';

$fp = fopen('phar://' . $fname . '/a', 'r');
$fp2 = fopen('phar://' . $fname . '/b', 'r');
$phar['c'] = 'cccc';

$streams = count(get_resources('stream'));
for ($i = 0; $i < 5; $i++) {
    $phar['d' . $i] = 'dddd';
}
var_dump(count(get_resources('stream')) === $streams);

var_dump(fclose($fp));
var_dump(fclose($fp2));
var_dump(count($phar));
var_dump($phar['c']->getContent());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
int(8)
string(4) "cccc"
