--TEST--
InterBase: create test database
--SKIPIF--
<?php if (!extension_loaded("interbase")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?
/* $Id$ */

	$test_base = "ibase_test.tmp";
	$name = tempnam("","CREATEDB");
	$ftmp = fopen($name,"w");
	if (is_file($test_base))
		fwrite($ftmp,
        	"connect \"$test_base\";
            drop database;\n"
        );
   	fwrite($ftmp,
    	"create database \"$test_base\";
        create table test1 (
   		i integer,
   		c varchar(100)
   	);
   	commit;
   	insert into test1(i, c) values(1,  'test table created with isql');
   	exit;\n"
    );
   	fclose($ftmp);
	exec("isql -i $name 2>&1");
	unlink($name);
?>
--EXPECT--

