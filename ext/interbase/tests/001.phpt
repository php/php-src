--TEST--
InterBase: create test database
--SKIPIF--
<?php if (!extension_loaded("interbase")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?
/* $Id$ */

	// remember to give write permission to the ext/ibase/tests directory
	// to the user running interbase, otherwise db creation/deletion will  fail
	$test_base = dirname(__FILE__)."/ibase_test.tmp";
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
	// passthru gives us some output, allowing the test to pass
	// (actually, it passes after the first run when the db gets deleted/recreated)
	passthru("isql -i $name 2>&1");
	unlink($name);
?>
--EXPECT--
Use CONNECT or CREATE DATABASE to specify a database
Database:  "ext/interbase/tests/ibase_test.tmp"
