--TEST--
InterBase: connect, close and pconnect
--SKIPIF--
<?php if (!extension_loaded("interbase")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?
/* $Id$ */

    require(dirname(__FILE__)."/interbase.inc");
    
	$test_base = dirname(__FILE__)."/ibase_test.tmp";

	ibase_connect($test_base);
	out_table("test1");
	ibase_close();

	$con = ibase_connect($test_base);
	$pcon1 = ibase_pconnect($test_base);
	$pcon2 = ibase_pconnect($test_base);
	ibase_close($con);
	ibase_close($pcon1);

	out_table("test1");

	ibase_close($pcon2);
?>
--EXPECT--
--- test1 ---
1	test table created with isql	
---
--- test1 ---
1	test table created with isql	
---
