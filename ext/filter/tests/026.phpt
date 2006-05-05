--TEST--
filter_data() and FILTER_SANITIZE_STRIPPED
--FILE--
<?php

var_dump(filter_data("<p>Let me <font color=\"#000000\">see</font> you <br /><b>Stripped</b> down to the bone</p>", FILTER_SANITIZE_STRIPPED));
var_dump(filter_data("!@#$%^&*()><<>+_\"'<br><p /><li />", FILTER_SANITIZE_STRIPPED));
var_dump(filter_data("", FILTER_SANITIZE_STRIPPED));

var_dump(filter_data("<p>Let me <font color=\"#000000\">see</font> you <br /><b>Stripped</b> down to the bone</p>", FILTER_SANITIZE_STRIPPED, FILTER_FLAG_STRIP_LOW));
var_dump(filter_data("!@#$%^&*()><<>+_\"'<br><p /><li />", FILTER_SANITIZE_STRIPPED, FILTER_FLAG_STRIP_LOW));
var_dump(filter_data("", FILTER_SANITIZE_STRIPPED, FILTER_FLAG_STRIP_LOW));

var_dump(filter_data("<p>Let me <font color=\"#000000\">see</font> you <br /><b>Stripped</b> down to the bone</p>", FILTER_SANITIZE_STRIPPED, FILTER_FLAG_STRIP_HIGH));
var_dump(filter_data("!@#$%^&*()><<>+_\"'<br><p /><li />", FILTER_SANITIZE_STRIPPED, FILTER_FLAG_STRIP_HIGH));
var_dump(filter_data("", FILTER_SANITIZE_STRIPPED, FILTER_FLAG_STRIP_HIGH));

echo "Done\n";
?>
--EXPECTF--	
string(40) "Let me see you Stripped down to the bone"
string(11) "!@#$%^&*()>"
string(0) ""
string(40) "Let me see you Stripped down to the bone"
string(11) "!@#$%^&*()>"
string(0) ""
string(40) "Let me see you Stripped down to the bone"
string(11) "!@#$%^&*()>"
string(0) ""
Done
