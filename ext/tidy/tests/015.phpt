--TEST--
Passing configuration options through tidy_parse_file().
--EXTENSIONS--
tidy
--FILE--
<?php
        $tidy = tidy_parse_file(__DIR__."/015.html", array('show-body-only'=>true));
        tidy_clean_repair($tidy);
        echo tidy_get_output($tidy);

?>
--EXPECT--
<b>testing</b>
