--TEST--
ResourceBundle double construction should not be allowed
--EXTENSIONS--
intl
--FILE--
<?php

include "resourcebundle.inc";
$r = new ResourceBundle('en_US', BUNDLE);
try {
    $r->__construct('en_US', BUNDLE);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: ResourceBundle object is already constructed
