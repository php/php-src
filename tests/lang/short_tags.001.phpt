--TEST--
short_open_tag: On
--INI--
short_open_tag=on
--FILE--
<?
echo "Used a short tag\n";
?>
Finished
--EXPECT--
Deprecated: Directive 'short_open_tag' is deprecated in Unknown on line 0
Used a short tag
Finished
