--TEST--
Parsing a file using constructor
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php
        $tidy = new tidy("ext/tidy/tests/013.html", array("show-body-only"=>true));
        $tidy->cleanRepair();
        echo $tidy;

?>
--EXPECT--
<b>testing</b>
