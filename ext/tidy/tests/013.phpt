--TEST--
Parsing a file using constructor
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php
        $tidy = new tidy("ext/tidy/tests/013.html", array("show-body-only"=>true));
        $tidy->clean_repair();
        echo $tidy;

?>
--EXPECT--
<b>testing</b>