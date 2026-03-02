--TEST--
Phar: #73035 (Out of bound when verify signature of tar phar in phar_parse_tarfile)
--EXTENSIONS--
phar
--FILE--
<?php
chdir(__DIR__);
try {
$phar = new PharData('bug73035.tar');
var_dump($phar);
} catch(UnexpectedValueException $e) {
    print $e->getMessage()."\n";
}
?>
DONE
--EXPECTF--
phar error: tar-based phar "%sbug73035.tar" signature cannot be read
DONE
