--TEST--
enchant_broker_init() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--EXTENSIONS--
enchant
--FILE--
<?php
$broker = enchant_broker_init();
if (is_object($broker)) {
    echo("OK\n");
} else {
    echo("failure, its not a resource\n");
}
?>
--EXPECT--
OK
