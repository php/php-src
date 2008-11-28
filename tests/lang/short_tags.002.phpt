--TEST--
short tags
--INI--
short_tags=off
--FILE--
<?
echo "Used a short tag\n";
?>
Finished
--EXPECT--
Used a short tag
Finished
