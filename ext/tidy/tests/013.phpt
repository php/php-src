--TEST--
Parsing a file using constructor
--EXTENSIONS--
tidy
--FILE--
<?php
        $tidy = new tidy(__DIR__."/013.html", array("show-body-only"=>true));
        $tidy->cleanRepair();
        echo $tidy;

?>
--EXPECT--
<b>testing</b>
