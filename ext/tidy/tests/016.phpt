--TEST--
Passing configuration file through tidy_parse_file() (may fail with buggy libtidy)
--EXTENSIONS--
tidy
--FILE--
<?php
        $tidy = tidy_parse_file(__DIR__."/016.html",
                                __DIR__."/016.tcfg");
        $cfg = $tidy->getConfig();
        echo $cfg["clean"];
?>
--EXPECT--
1
