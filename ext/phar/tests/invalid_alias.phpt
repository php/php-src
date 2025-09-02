--TEST--
Phar: set alias with invalid alias containing / \ : or ;
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';

$p = new Phar($fname);
try {
    $p->setAlias('hi/');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
try {
    $p->setAlias('hi\\l');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    $p->setAlias('hil;');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    $p->setAlias(':hil');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
Invalid alias "hi/" specified for phar "%sinvalid_alias.phar"
Invalid alias "hi\l" specified for phar "%sinvalid_alias.phar"
Invalid alias "hil;" specified for phar "%sinvalid_alias.phar"
Invalid alias ":hil" specified for phar "%sinvalid_alias.phar"
