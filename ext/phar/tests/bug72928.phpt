--TEST--
Phar: #72928 (Out of bound when verify signature of zip phar in phar_parse_zipfile)
--EXTENSIONS--
phar
--FILE--
<?php
chdir(__DIR__);
try {
$phar = new PharData('bug72928.zip');
var_dump($phar);
} catch(UnexpectedValueException $e) {
    print $e->getMessage()."\n";
}
?>
DONE
--EXPECTF--
phar error: signature cannot be read in zip-based phar "%sbug72928.zip"
DONE
