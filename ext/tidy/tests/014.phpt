--TEST--
Passing configuration options through tidy_parse_string().
--EXTENSIONS--
tidy
--FILE--
<?php
        $text = "<B>testing</I>";
        $tidy = tidy_parse_string($text, array('show-body-only'=>true));
        tidy_clean_repair($tidy);
        echo tidy_get_output($tidy);

?>
--EXPECT--
<b>testing</b>
