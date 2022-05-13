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
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
ResourceBundle object is already constructed
