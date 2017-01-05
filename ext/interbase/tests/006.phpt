--TEST--
InterBase: binding (may take a while)
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

	require("interbase.inc");
	
	ibase_connect($test_base);
	
	ibase_query(
		"create table test6 (
			iter		integer,
			v_char		char(1000),
			v_date      timestamp,
			v_decimal   decimal(12,3),
			v_double  	double precision,
			v_float     float,
			v_integer   integer,
			v_numeric   numeric(4,2),
			v_smallint  smallint,
			v_varchar   varchar(10000)
			)");
	ibase_query(
		"create procedure add1 (arg integer)
		returns (result integer)
		as 
		begin
			result = arg +1;
		end");
	ibase_commit();

	/* if timefmt not supported, hide error */
	ini_set('ibase.timestampformat',"%m/%d/%Y %H:%M:%S");

	echo "insert\n";

	for($iter = 0; $iter < 3; $iter++) {
		/* prepare data  */
		$v_char = rand_str(1000);
		$v_date = rand_datetime();
		$v_decimal = rand_number(12,3);
		$v_double  = rand_number(20);
		$v_float   = rand_number(7);
		$v_integer = rand_number(9,0);
		$v_numeric = rand_number(4,2);
		$v_smallint = rand_number(5) % 32767;
		$v_varchar = rand_str(10000);

		ibase_query("insert into test6
			(iter,v_char,v_date,v_decimal,v_double,v_float,
			v_integer,v_numeric,v_smallint,v_varchar)
			values (?,?,?,?,?,?,?,?,?,?)",
			$iter, $v_char, $v_date, $v_decimal, $v_double, $v_float,
			$v_integer, $v_numeric, $v_smallint, $v_varchar);
		$sel = ibase_query("select * from test6 where iter = ?", $iter);

		$row = ibase_fetch_object($sel);
		if(substr($row->V_CHAR,0,strlen($v_char)) != $v_char) {
			echo " CHAR fail:\n";
			echo " in:  $v_char\n";
			echo " out: $row->V_CHAR\n";
		}
		if($row->V_DATE != $v_date) {
			echo " DATE fail\n";
			echo " in:  $v_date\n";
			echo " out: $row->V_DATE\n";
		}
		if($row->V_DECIMAL != $v_decimal) {
			echo " DECIMAL fail\n";
			echo " in:  $v_decimal\n";
			echo " out: $row->V_DECIMAL\n";
		}
		if(abs($row->V_DOUBLE - $v_double) > abs($v_double / 1E15)) {
			echo " DOUBLE fail\n";
			echo " in:  $v_double\n";
			echo " out: $row->V_DOUBLE\n";
		}
		if(abs($row->V_FLOAT - $v_float) > abs($v_float / 1E7)) {
			echo " FLOAT fail\n";
			echo " in:  $v_float\n";
			echo " out: $row->V_FLOAT\n";
		}
		if($row->V_INTEGER != $v_integer) {
			echo " INTEGER fail\n";
			echo " in:  $v_integer\n";
			echo " out: $row->V_INTEGER\n";
		}
		if ($row->V_NUMERIC != $v_numeric) {
			echo " NUMERIC fail\n";
			echo " in:  $v_numeric\n";
			echo " out: $row->V_NUMERIC\n";
		}
		if ($row->V_SMALLINT != $v_smallint) {
			echo " SMALLINT fail\n";
			echo " in:  $v_smallint\n";
			echo " out: $row->V_SMALLINT\n";
		}
		if ($row->V_VARCHAR != $v_varchar) {
			echo " VARCHAR fail:\n";
			echo " in:  $v_varchar\n";
			echo " out: $row->V_VARCHAR\n";
		}
		ibase_free_result($sel);
	}/* for($iter)*/

	echo "select\n";
	for($iter = 0; $iter < 3; $iter++) {
		/* prepare data  */
		$v_char = rand_str(1000);
		$v_date = (int)rand_number(10,0,0);
		$v_decimal = rand_number(12,3);
		$v_double  = rand_number(20);
		$v_float   = rand_number(7);
		$v_integer = rand_number(9,0);
		$v_numeric = rand_number(4,2);
		$v_smallint = rand_number(5) % 32767;
		$v_varchar = rand_str(10000);

		/* clear table*/
		ibase_query("delete from test6");

		/* make one record */
		ibase_query("insert into test6
			(iter, v_char,v_date,v_decimal,
			v_integer,v_numeric,v_smallint,v_varchar)
			values (666, '$v_char',?,$v_decimal, $v_integer,
			$v_numeric, $v_smallint, '$v_varchar')",$v_date);

		/* test all types */
		if(!($sel = ibase_query(
			"select iter from test6 where v_char = ?", $v_char)) ||
			!ibase_fetch_row($sel)) {
			echo "CHAR fail\n";
		}
		ibase_free_result($sel);
		if(!($sel = ibase_query(
			"select iter from test6 where v_date = ?", $v_date)) ||
			!ibase_fetch_row($sel)) {
			echo "DATE fail\n";
		}
		ibase_free_result($sel);
		if(!($sel = ibase_query(
			"select iter from test6 where v_decimal = ?", $v_decimal)) ||
			!ibase_fetch_row($sel)) {
			echo "DECIMAL fail\n";
		}
		ibase_free_result($sel);
		if(!($sel = ibase_query(
			"select iter from test6 where v_integer = ?", $v_integer)) ||
			!ibase_fetch_row($sel)) {
			echo "INTEGER fail\n";
		}
		ibase_free_result($sel);
		if(!($sel = ibase_query(
			"select iter from test6 where v_numeric = ?", $v_numeric)) ||
			!ibase_fetch_row($sel)) {
			echo "NUMERIC fail\n";
		}
		ibase_free_result($sel);
		if(!($sel = ibase_query(
			"select iter from test6 where v_smallint = ?", $v_smallint)) ||
			!ibase_fetch_row($sel)) {
			echo "SMALLINT fail\n";
		}
		ibase_free_result($sel);
		if(!($sel = ibase_query(
			"select iter from test6 where v_varchar = ?", $v_varchar)) ||
			!ibase_fetch_row($sel)) {
			echo "VARCHAR fail\n";
		}
		ibase_free_result($sel);
        
	} /*for iter*/

	echo "prepare and exec insert\n";

    /* prepare table */
	ibase_query("delete from test6");

	/* prepare query */
	$query = ibase_prepare(
		"insert into test6 (v_integer) values (?)");

	for($i = 0; $i < 10; $i++) {
		ibase_execute($query, $i);
	}

	out_table("test6");

	ibase_free_query($query);

	echo "prepare and exec select\n";

	/* prepare query */
	$query = ibase_prepare("select * from test6
		where v_integer between ? and ?");

	$low_border = 2;
	$high_border = 6;

	$res = ibase_execute($query, $low_border, $high_border);
	out_result($res, "test6");
	ibase_free_result($res);

	$low_border = 0;
	$high_border = 4;
	$res = ibase_execute($query, $low_border, $high_border);
	out_result($res, "test6");
	ibase_free_result($res);

	$res = ibase_execute($query, "5", 7.499);
	out_result($res, "test6");
	ibase_free_result($res);

	ibase_free_query($query);

	/* test execute procedure */
	$query = ibase_prepare("execute procedure add1(?)");
	$res = array();
	for ($i = 0; $i < 10; $i++) {
		$res[] = ibase_execute($query,$i);
	}
	ibase_free_query($query);
	foreach ($res as $r) {
		out_result($r, "proc add1");
		ibase_free_result($r);
	}

	ibase_close();
	echo "end of test\n";
?>
--EXPECT--
insert
select
prepare and exec insert
--- test6 ---
						0				
						1				
						2				
						3				
						4				
						5				
						6				
						7				
						8				
						9				
---
prepare and exec select
--- test6 ---
						2				
						3				
						4				
						5				
						6				
---
--- test6 ---
						0				
						1				
						2				
						3				
						4				
---
--- test6 ---
						5				
						6				
						7				
---
--- proc add1 ---
1	
---
--- proc add1 ---
2	
---
--- proc add1 ---
3	
---
--- proc add1 ---
4	
---
--- proc add1 ---
5	
---
--- proc add1 ---
6	
---
--- proc add1 ---
7	
---
--- proc add1 ---
8	
---
--- proc add1 ---
9	
---
--- proc add1 ---
10	
---
end of test

