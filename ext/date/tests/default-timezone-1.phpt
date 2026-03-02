--TEST--
date.timezone setting [1]
--FILE--
<?php
date_default_timezone_set('GMT');
    putenv('TZ='); // clean TZ so that it doesn't bypass the ini option
    echo strtotime("2005-06-18 22:15:44");
?>
--EXPECT--
1119132944
