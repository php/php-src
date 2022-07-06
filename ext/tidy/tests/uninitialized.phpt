--TEST--
Operations on uninitialized tidy object
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php

$tidy = new tidy;
try {
    var_dump($tidy->getHtmlVer());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($tidy->isXhtml());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($tidy->isXml());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
tidy object is not initialized
tidy object is not initialized
tidy object is not initialized
