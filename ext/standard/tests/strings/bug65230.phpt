--TEST--
Bug #65230 setting locale randomly broken
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip');
}
?>
--INI--
date.timezone=Europe/Berlin
--FILE--
<?php

function test($locale, $value) 
{
	$newlocale = setlocale(LC_ALL, $locale);
	$conv      = localeconv();
	$sep       = $conv['decimal_point'];

	printf("%s\n--------------------------\n", $newlocale);
	printf(" sep: %s\n", $sep);
	printf("  %%f: %f\n", $value);
	printf("  %%F: %F\n", $value);
	printf("date: %s\n", strftime('%x', mktime(0, 0, 0, 12, 5, 2014)));
	printf("\n");
}

test('german', 3.41);
test('english', 3.41);
test('french', 3.41);
test('german', 3.41);
--EXPECT--
German_Germany.1252
--------------------------
 sep: ,
  %f: 3,410000
  %F: 3.410000
date: 05.12.2014

English_United States.1252
--------------------------
 sep: .
  %f: 3.410000
  %F: 3.410000
date: 12/5/2014

French_France.1252
--------------------------
 sep: ,
  %f: 3,410000
  %F: 3.410000
date: 05/12/2014

German_Germany.1252
--------------------------
 sep: ,
  %f: 3,410000
  %F: 3.410000
date: 05.12.2014

