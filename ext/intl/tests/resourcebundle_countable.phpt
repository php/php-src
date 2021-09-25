--TEST--
Test ResourceBundle implements Countable
--EXTENSIONS--
intl
--FILE--
<?php
    include "resourcebundle.inc";

    $r = new ResourceBundle( 'es', BUNDLE );

    var_dump($r instanceof Countable);
?>
--EXPECT--
bool(true)
