--TEST--
short_open_tag: On
--INI--
short_open_tag=on
--FILE--
<?
echo "Used a short tag\n";
?>
Finished
--EXPECTF--
Deprecated: PHP short tags are deprecated. This message will not be printed for further short tags uses. First usage in %s on line %d
Used a short tag
Finished
