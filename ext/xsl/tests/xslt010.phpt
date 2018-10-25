--TEST--
Test 10: EXSLT Support
--SKIPIF--
<?php
require_once dirname(__FILE__) .'/skipif.inc';
$proc = new xsltprocessor;
if (!$proc->hasExsltSupport()) die('skip EXSLT support not available');
if (LIBXSLT_VERSION < 10117) die('skip too old libxsl');
if (LIBXSLT_VERSION > 10129) die('skip for libxsl < 10130');
?>
--FILE--
<?php
echo "Test 10: EXSLT Support";

$dom = new domDocument();
  $dom->load(dirname(__FILE__)."/exslt.xsl");
  $proc = new xsltprocessor;
  $xsl = $proc->importStylesheet($dom);

  $xml = new DomDocument();
  $xml->load(dirname(__FILE__)."/exslt.xml");

  print $proc->transformToXml($xml);
--EXPECT--
Test 10: EXSLT Support
  
  
  Test Date : 0001-12-31Z
    year                 : 1
    leap-year            : false
    month-in-year        : 12
    month-name           : December
    month-abbreviation   : Dec
    week-in-year         : 1
    day-in-year          : 365
    day-in-month         : 31
    day-of-week-in-month : 5
    day-in-week          : 2
    day-name             : Monday
    day-abbreviation     : Mon
    time                 : 
    hour-in-day          : NaN
    minute-in-hour       : NaN
    second-in-minute     : NaN
  
  Test Date : 3000-01-31
    year                 : 3000
    leap-year            : false
    month-in-year        : 1
    month-name           : January
    month-abbreviation   : Jan
    week-in-year         : 5
    day-in-year          : 31
    day-in-month         : 31
    day-of-week-in-month : 5
    day-in-week          : 6
    day-name             : Friday
    day-abbreviation     : Fri
    time                 : 
    hour-in-day          : NaN
    minute-in-hour       : NaN
    second-in-minute     : NaN
  
  Test Date : 2000-02-29
    year                 : 2000
    leap-year            : true
    month-in-year        : 2
    month-name           : February
    month-abbreviation   : Feb
    week-in-year         : 9
    day-in-year          : 60
    day-in-month         : 29
    day-of-week-in-month : 5
    day-in-week          : 3
    day-name             : Tuesday
    day-abbreviation     : Tue
    time                 : 
    hour-in-day          : NaN
    minute-in-hour       : NaN
    second-in-minute     : NaN
  
  Test Date : 9990001-12-31Z
    year                 : 9990001
    leap-year            : false
    month-in-year        : 12
    month-name           : December
    month-abbreviation   : Dec
    week-in-year         : 1
    day-in-year          : 365
    day-in-month         : 31
    day-of-week-in-month : 5
    day-in-week          : 2
    day-name             : Monday
    day-abbreviation     : Mon
    time                 : 
    hour-in-day          : NaN
    minute-in-hour       : NaN
    second-in-minute     : NaN
  
  Test Date : -0004-02-29
    year                 : -4
    leap-year            : true
    month-in-year        : 2
    month-name           : February
    month-abbreviation   : Feb
    week-in-year         : 9
    day-in-year          : 60
    day-in-month         : 29
    day-of-week-in-month : 5
    day-in-week          : 1
    day-name             : Sunday
    day-abbreviation     : Sun
    time                 : 
    hour-in-day          : NaN
    minute-in-hour       : NaN
    second-in-minute     : NaN
  
  Test Date : 1999-01-02
    year                 : 1999
    leap-year            : false
    month-in-year        : 1
    month-name           : January
    month-abbreviation   : Jan
    week-in-year         : 53
    day-in-year          : 2
    day-in-month         : 2
    day-of-week-in-month : 1
    day-in-week          : 7
    day-name             : Saturday
    day-abbreviation     : Sat
    time                 : 
    hour-in-day          : NaN
    minute-in-hour       : NaN
    second-in-minute     : NaN
  
  Test Date : 1999-01-03
    year                 : 1999
    leap-year            : false
    month-in-year        : 1
    month-name           : January
    month-abbreviation   : Jan
    week-in-year         : 53
    day-in-year          : 3
    day-in-month         : 3
    day-of-week-in-month : 1
    day-in-week          : 1
    day-name             : Sunday
    day-abbreviation     : Sun
    time                 : 
    hour-in-day          : NaN
    minute-in-hour       : NaN
    second-in-minute     : NaN
  
  Test Date : 2004-01-01
    year                 : 2004
    leap-year            : true
    month-in-year        : 1
    month-name           : January
    month-abbreviation   : Jan
    week-in-year         : 1
    day-in-year          : 1
    day-in-month         : 1
    day-of-week-in-month : 1
    day-in-week          : 5
    day-name             : Thursday
    day-abbreviation     : Thu
    time                 : 
    hour-in-day          : NaN
    minute-in-hour       : NaN
    second-in-minute     : NaN
  
  Test Date : 2006-01-01
    year                 : 2006
    leap-year            : false
    month-in-year        : 1
    month-name           : January
    month-abbreviation   : Jan
    week-in-year         : 52
    day-in-year          : 1
    day-in-month         : 1
    day-of-week-in-month : 1
    day-in-week          : 1
    day-name             : Sunday
    day-abbreviation     : Sun
    time                 : 
    hour-in-day          : NaN
    minute-in-hour       : NaN
    second-in-minute     : NaN
  
  Test Date : 2007-12-31
    year                 : 2007
    leap-year            : false
    month-in-year        : 12
    month-name           : December
    month-abbreviation   : Dec
    week-in-year         : 1
    day-in-year          : 365
    day-in-month         : 31
    day-of-week-in-month : 5
    day-in-week          : 2
    day-name             : Monday
    day-abbreviation     : Mon
    time                 : 
    hour-in-day          : NaN
    minute-in-hour       : NaN
    second-in-minute     : NaN
