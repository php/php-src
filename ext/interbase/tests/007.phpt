--TEST--
InterBase: array handling
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php /* $Id$ */

	require("interbase.inc");
	
	ibase_connect($test_base);
	
	ibase_query(
		"create table test7 (
			iter		integer,
			v_multi		integer[10,10,10],
			v_char		char(100)[10],
			v_date      timestamp[10],
			v_decimal   decimal(18,3)[10],
			v_double  	double precision[10],
			v_float     float[10],
			v_integer   integer[10],
			v_numeric   numeric(9,2)[10],
			v_smallint  smallint[10],
			v_varchar   varchar(1000)[10]
			)");
	ibase_commit();

	/* if timefmt not supported, hide error */
	ini_set('ibase.timestampformat',"%m/%d/%Y %H:%M:%S");

	echo "insert\n";
	
	for ($i = 1; $i <= 10; ++$i) {
		for ($j = 1; $j <= 10; ++$j) {
			for ($k = 1; $k <= 10; ++$k) {
				$v_multi[$i][$j][$k] = $i * $j * $k;
			}
		}
	}
				
	for($iter = 0; $iter < 3; $iter++) {

		/* prepare data  */
		$v_char = array();
		$v_date = array();
		$v_decimal = array();
		$v_double  = array();
		$v_float = array();
		$v_integer = array();
		$v_numeric = array();
		$v_smallint = array();
		$v_varchar = array();

		for ($i = 1; $i <= 10; ++$i) {
			$v_char[$i] = rand_str(100);
			$v_date[$i] = rand_datetime();
			$v_decimal[$i] = rand_number(18,3);
			$v_double[$i]  = rand_number(20);
			$v_float[$i]   = rand_number(7);
			$v_integer[$i] = rand_number(9,0);
			$v_numeric[$i] = rand_number(9,2);
			$v_smallint[$i] = rand_number(5) % 32767;
			$v_varchar[$i] = rand_str(1000);
		}
		
		ibase_query("insert into test7
			(iter,v_multi,v_char,v_date,v_decimal,v_double,v_float,
			v_integer,v_numeric,v_smallint,v_varchar)
			values (?,?,?,?,?,?,?,?,?,?,?)",
			$iter, $v_multi, $v_char, $v_date, $v_decimal, $v_double, $v_float,
			$v_integer, $v_numeric, $v_smallint, $v_varchar);
		$sel = ibase_query("select * from test7 where iter = $iter");

		$row = ibase_fetch_object($sel,IBASE_FETCH_ARRAYS);
		for ($i = 1; $i <= 10; ++$i) {
			
			if(strncmp($row->V_CHAR[$i],$v_char[$i],strlen($v_char[$i])) != 0) {
				echo " CHAR[$i] fail:\n";
				echo " in:  ".$v_char[$i]."\n";
				echo " out: ".$row->V_CHAR[$i]."\n";
			}
			if($row->V_DATE[$i] != $v_date[$i]) {
				echo " DATE[$i] fail\n";
				echo " in:  ".$v_date[$i]."\n";
				echo " out: ".$row->V_DATE[$i]."\n";
			}
			if($row->V_DECIMAL[$i] != $v_decimal[$i]) {
				echo " DECIMAL[$i] fail\n";
				echo " in:  ".$v_decimal[$i]."\n";
				echo " out: ".$row->V_DECIMAL[$i]."\n";
			}
			if(abs($row->V_DOUBLE[$i] - $v_double[$i]) > abs($v_double[$i] / 1E15)) {
				echo " DOUBLE[$i] fail\n";
				echo " in:  ".$v_double[$i]."\n";
				echo " out: ".$row->V_DOUBLE[$i]."\n";
			}
			if(abs($row->V_FLOAT[$i] - $v_float[$i]) > abs($v_float[$i] / 1E7)) {
				echo " FLOAT[$i] fail\n";
				echo " in:  ".$v_float[$i]."\n";
				echo " out: ".$row->V_FLOAT[$i]."\n";
			}
			if($row->V_INTEGER[$i] != $v_integer[$i]) {
				echo " INTEGER[$i] fail\n";
				echo " in:  ".$v_integer[$i]."\n";
				echo " out: ".$row->V_INTEGER[$i]."\n";
			}
			if ($row->V_NUMERIC[$i] != $v_numeric[$i]) {
				echo " NUMERIC[$i] fail\n";
				echo " in:  ".$v_numeric[$i]."\n";
				echo " out: ".$row->V_NUMERIC[$i]."\n";
			}
			if ($row->V_SMALLINT[$i] != $v_smallint[$i]) {
				echo " SMALLINT[$i] fail\n";
				echo " in:  ".$v_smallint[$i]."\n";
				echo " out: ".$row->V_SMALLINT[$i]."\n";
			}
			if ($row->V_VARCHAR[$i] != $v_varchar[$i]) {
				echo " VARCHAR[$i] fail:\n";
				echo " in:  ".$v_varchar[$i]."\n";
				echo " out: ".$row->V_VARCHAR[$i]."\n";
			}
		}
		ibase_free_result($sel);
	}/* for($iter) */

	echo "select\n";
	
	$sel = ibase_query("SELECT v_multi[5,5,5],v_multi[10,10,10] FROM test7 WHERE iter = 0");
	print_r(ibase_fetch_row($sel));
	ibase_free_result($sel);

	for($iter = 1; $iter <= 3; $iter++) {

		if(!($sel = ibase_query(
			"select iter from test7 where v_char[$iter] LIKE ?", $v_char[$iter]."%")) ||
			!ibase_fetch_row($sel)) {
			echo "CHAR fail\n";
		}
		ibase_free_result($sel);

		if(!($sel = ibase_query(
			"select iter from test7 where v_date[$iter] = ?", $v_date[$iter])) ||
			!ibase_fetch_row($sel)) {
			echo "DATE fail\n";
		}
		ibase_free_result($sel);
		if(!($sel = ibase_query(
			"select iter from test7 where v_decimal[$iter] = ?", $v_decimal[$iter])) ||
			!ibase_fetch_row($sel)) {
			echo "DECIMAL fail\n";
		}
		ibase_free_result($sel);
		if(!($sel = ibase_query(
			"select iter from test7 where v_integer[$iter] = ?", $v_integer[$iter])) ||
			!ibase_fetch_row($sel)) {
			echo "INTEGER fail\n";
		}
		ibase_free_result($sel);
		if(!($sel = ibase_query(
			"select iter from test7 where v_numeric[$iter] = ?", $v_numeric[$iter])) ||
			!ibase_fetch_row($sel)) {
			echo "NUMERIC fail\n";
		}
		ibase_free_result($sel);
		if(!($sel = ibase_query(
			"select iter from test7 where v_smallint[$iter] = ?", $v_smallint[$iter])) ||
			!ibase_fetch_row($sel)) {
			echo "SMALLINT fail\n";
		}
		ibase_free_result($sel);
	}
	ibase_close();
	echo "end of test\n";
?>
--EXPECT--
insert
select
Array
(
    [0] => 125
    [1] => 1000
)
end of test
