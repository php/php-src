--TEST--
Passing configuration file through tidy_parse_file() (may fail with buggy libtidy)
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php
        $tidy = tidy_parse_file(dirname(__FILE__)."/016.html", dirname(__FILE__)."/016.tcfg");
    	tidy_clean_repair($tidy);
        echo tidy_get_output($tidy);
?>
--EXPECT--
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 3.2//EN">
<html>
<head>
<title></title>

<style type="text/css">
 p.c1 {font-weight: bold}
</style>
</head>
<body>
<p class="c1">testing</p>
</body>
</html>
