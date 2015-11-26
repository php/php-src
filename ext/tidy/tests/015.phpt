--TEST--
Passing configuration options through tidy_parse_file().
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php
        $tidy = tidy_parse_file(dirname(__FILE__)."/015.html", array('show-body-only'=>true));
    	tidy_clean_repair($tidy);
    	echo tidy_get_output($tidy);
            
?>
--EXPECT--
<b>testing</b>