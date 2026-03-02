--TEST--
UConverter::transcode issue with substitutes values as references
--EXTENSIONS--
intl
--FILE--
<?php
$subst = '??';
$opts = array('from_subst' => '?', 'to_subst' => &$subst);
var_dump(UConverter::transcode("This is an ascii string", 'ascii', 'utf-8', $opts));
$opts = array('from_subst' => &$subst, 'to_subst' => '?');
var_dump(UConverter::transcode("This is an ascii string", 'ascii', 'utf-8', $opts));
// should yield the same results
$opts = array('from_subst' => '?', 'to_subst' => '??');
var_dump(UConverter::transcode("This is an ascii string", 'ascii', 'utf-8', $opts));
$opts = array('from_subst' => '??', 'to_subst' => '?');
var_dump(UConverter::transcode("This is an ascii string", 'ascii', 'utf-8', $opts));
?>
--EXPECT--
bool(false)
string(23) "This is an ascii string"
bool(false)
string(23) "This is an ascii string"
