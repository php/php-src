--TEST--
Test ResourceBundle implements Countable
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
    include "resourcebundle.inc";

    $r = new ResourceBundle( 'es', BUNDLE );

    var_dump($r instanceof Countable);
?>
--EXPECT--
bool(true)
