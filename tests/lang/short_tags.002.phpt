--TEST--
short_open_tag: Off
--INI--
short_open_tag=off
--FILE--
<?
echo "Used a short tag\n";
?>
Finished
--EXPECT--
<?
echo "Used a short tag\n";
?>
Finished
