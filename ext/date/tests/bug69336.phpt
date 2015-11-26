--TEST--
Bug #69336 (Issues with "last day of <monthname>")
--INI--
date.timezone=UTC
--FILE--
<?php
var_dump(date('d.m.Y',strtotime('last day of april')));
var_dump(date('d.m.Y',strtotime('last tuesday of march 2015')));
var_dump(date('d.m.Y',strtotime('last wednesday of march 2015')));
var_dump(date('d.m.Y',strtotime('last wednesday of april 2015')));
var_dump(date('d.m.Y',strtotime('last wednesday of march 2014')));
var_dump(date('d.m.Y',strtotime('last wednesday of april 2014')));
?>
--EXPECTF--
string(10) "30.04.%d"
string(10) "31.03.2015"
string(10) "25.03.2015"
string(10) "29.04.2015"
string(10) "26.03.2014"
string(10) "30.04.2014"
