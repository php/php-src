--TEST--
Passing configuration file through tidy_parse_file() (may fail with buggy libtidy)
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php
        $tidy = tidy_parse_file(dirname(__FILE__)."/016.html",
                                dirname(__FILE__)."/016.tcfg");
        $cfg = $tidy->getConfig();
        echo $cfg["clean"];
?>
--EXPECT--
1
