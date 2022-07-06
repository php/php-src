--TEST--
Bug #37616 (DATE_RFC822 does not product RFC 822 dates)
--FILE--
<?php
    date_default_timezone_set("Europe/Oslo");
    var_dump(date(DATE_RFC822, strtotime("1 Jul 06 14:27:30 +0200")));
?>
--EXPECT--
string(29) "Sat, 01 Jul 06 14:27:30 +0200"
