--TEST--
Operations on uninitialized tidy object
--EXTENSIONS--
tidy
--FILE--
<?php

$tidy = new tidy;
try {
    var_dump($tidy->getHtmlVer());
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($tidy->isXhtml());
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($tidy->isXml());
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: tidy object is not initialized
Error: tidy object is not initialized
Error: tidy object is not initialized
