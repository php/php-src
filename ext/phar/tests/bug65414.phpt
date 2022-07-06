--TEST--
Bug #65414 Injection (A1) in .phar files magic .phar directory
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly = 0
--FILE--
<?php
$phar = new \Phar(__DIR__ . '/bug65414.phar', 0, 'bug65414.phar');
$bads = [
    '.phar/injected-1.txt',
    '/.phar/injected-2.txt',
    '//.phar/injected-3.txt',
    '/.phar/',
];
foreach ($bads as $bad) {
    echo $bad . ':';
    try {
        $phar->addFromString($bad, 'this content is injected');
        echo 'Failed to throw expected exception';
    } catch (BadMethodCallException $ex) {
        echo $ex->getMessage() . PHP_EOL;
    }
}
echo 'done' . PHP_EOL;
?>
--CLEAN--
<?php
unlink(__DIR__ . '/bug65414.phar');
?>
--EXPECT--
.phar/injected-1.txt:Cannot create any files in magic ".phar" directory
/.phar/injected-2.txt:Cannot create any files in magic ".phar" directory
//.phar/injected-3.txt:Entry //.phar/injected-3.txt does not exist and cannot be created: phar error: invalid path "//.phar/injected-3.txt" contains double slash
/.phar/:Cannot create any files in magic ".phar" directory
done
