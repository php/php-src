--TEST--
MessageFormatter::format(): simple types handling with numeric strings
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
//ini_set("intl.default_locale", "nl");

$mf = new MessageFormatter('en_US',"
	none			{a}
	number			{b,number}
	number integer	{c,number,integer}
	number currency	{d,number,currency}
	number percent	{e,number,percent}
	date			{f,date}
	time			{g,time}
	spellout		{h,spellout}
	ordinal			{i,ordinal}
	duration		{j,duration}
	");

$ex = "1336317965.5 str";
var_dump($mf->format(array(
'a' => $ex,
'b' => $ex,
'c' => $ex,
'd' => $ex,
'e' => $ex,
'f' => "  1336317965.5",
'g' => "  1336317965.5",
'h' => $ex,
'i' => $ex,
'j' => $ex,
)));

?>
==DONE==
--EXPECTF--
string(%d) "
	none			1336317965.5 str
	number			1,336,317,965.5
	number integer	1,336,317,965
	number currency	$1,336,317,965.50
	number percent	133,631,796,550%
	date			May %d, 2012
	time			%d:%d:05 PM
	spellout		one billion three hundred thirty-six million three hundred seventeen thousand nine hundred sixty-five point five
	ordinal			1,336,317,966th
	duration		371,199:26:06
	"
==DONE==
