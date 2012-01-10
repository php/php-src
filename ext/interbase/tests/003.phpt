--TEST--
InterBase: misc sql types (may take a while)
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php /* $Id$ */

    require("interbase.inc");
    ibase_connect($test_base);
    
    ibase_query(
    	"create table test3 (
            iter		integer not null,
            v_char		char(1000),
            v_date      timestamp,
   	 		v_decimal4_2  		decimal(4,2),
   	 		v_decimal4_0  		decimal(4,0),
   	 		v_decimal7_2  		decimal(7,2),
   	 		v_decimal7_0  		decimal(7,0),
   	 		v_numeric15_15  	numeric(15,15),
   	 		v_numeric15_0  	numeric(15,0),
            v_double  	double precision,
            v_float     float,
            v_integer   integer,
            v_smallint  smallint,
            v_varchar   varchar(10000)
            )");
    ibase_commit();

	/* should fail, but gracefully */
	@ibase_query("insert into test3 (iter) values (?)", null);

    /* if timefmt is not supported, suppress error here */
    ini_set('ibase.timestampformat',"%m/%d/%Y %H:%M:%S");

    for($iter = 0; $iter < 10; $iter++){
    	/* prepare data  */
    	$v_char = rand_str(1000);
    	$v_date = rand_datetime();
    	$v_decimal4_2 = rand_number(4,2);
    	$v_decimal4_0 = rand_number(4,0);
    	$v_decimal7_2 = rand_number(7,2);
    	$v_decimal7_0 = rand_number(7,0);
    	$v_numeric15_15 = rand_number(15,15);
    	$v_numeric15_0 = $iter ? rand_number(15,0) : 0;
    	$v_double  = rand_number(18);
    	$v_float   = rand_number(7);
    	$v_integer = rand_number(9,0);
    	$v_smallint = rand_number(5) % 32767;
    	$v_varchar = rand_str(10000);

    	ibase_query(
    	"insert into test3 (iter, v_char,v_date,v_decimal4_2, v_decimal4_0, v_decimal7_2, v_decimal7_0,v_numeric15_15, v_numeric15_0,v_double,v_float,v_integer,v_smallint,v_varchar)
    	values ($iter, '$v_char','$v_date',$v_decimal4_2, $v_decimal4_0, $v_decimal7_2, $v_decimal7_0,$v_numeric15_15, $v_numeric15_0,$v_double,$v_float,$v_integer,$v_smallint,'$v_varchar')");
    	$sel = ibase_query("select * from test3 where iter = $iter");
    	$row = ibase_fetch_object($sel);
    	if(substr($row->V_CHAR,0,strlen($v_char)) != $v_char){
        	echo " CHAR fail:\n";
            echo " in:  $v_char\n";
            echo " out: $row->V_CHAR\n";
        }
        if($row->V_DATE != $v_date){
            echo " DATE fail\n";
            echo " in:  $v_date\n";
            echo " out: $row->V_DATE\n";
        }
        if($row->V_DECIMAL4_2 != $v_decimal4_2){
            echo " DECIMAL4_2 fail\n";
            echo " in:  $v_decimal4_2\n";
            echo " out: $row->V_DECIMAL4_2\n";
        }
        if($row->V_DECIMAL4_0 != $v_decimal4_0){
            echo " DECIMAL4_0 fail\n";
            echo " in:  $v_decimal4_0\n";
            echo " out: $row->V_DECIMAL4_0\n";
        }
        if($row->V_DECIMAL7_2 != $v_decimal7_2){
            echo " DECIMAL7_2 fail\n";
            echo " in:  $v_decimal7_2\n";
            echo " out: $row->V_DECIMAL7_2\n";
        }
        if($row->V_DECIMAL7_0 != $v_decimal7_0){
            echo " DECIMAL7_0 fail\n";
            echo " in:  $v_decimal7_0\n";
            echo " out: $row->V_DECIMAL7_0\n";
        }
        if($row->V_NUMERIC15_15 != $v_numeric15_15){
            echo " NUMERIC15_15 fail\n";
            echo " in:  $v_numeric15_15\n";
            echo " out: $row->V_NUMERIC15_15\n";
        }
        if($row->V_NUMERIC15_0 != (string)$v_numeric15_0){
            echo " NUMERIC15_0 fail\n";
            echo " in:  $v_numeric15_0\n";
            echo " out: $row->V_NUMERIC15_0\n";
        }

        if(abs($row->V_DOUBLE - $v_double) > abs($v_double / 1E15)){
            echo " DOUBLE fail\n";
            echo " in:  $v_double\n";
            echo " out: $row->V_DOUBLE\n";
        }
        if(abs($row->V_FLOAT - $v_float) > abs($v_float / 1E7)){
            echo " FLOAT fail\n";
            echo " in:  $v_float\n";
            echo " out: $row->V_FLOAT\n";
        }
        if($row->V_INTEGER != $v_integer){
            echo " INTEGER fail\n";
            echo " in:  $v_integer\n";
            echo " out: $row->V_INTEGER\n";
        }
        if($row->V_SMALLINT != $v_smallint){
            echo " SMALLINT fail\n";
            echo " in:  $v_smallint\n";
            echo " out: $row->V_SMALLINT\n";
        }

	  if(substr($row->V_VARCHAR,0,strlen($v_varchar)) != $v_varchar){
        	echo " VARCHAR fail:\n";
            echo " in:  $v_varchar\n";
            echo " out: $row->V_VARCHAR\n";
        }

        ibase_free_result($sel);
    } /* for($iter) */

	/* check for correct handling of duplicate field names */
	$q = ibase_query('SELECT 1 AS id, 2 AS id, 3 AS id, 4 AS id, 5 AS id, 6 AS id, 7 AS id, 8 AS id, 9 AS id,
		10 AS id, 11 AS id, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 FROM rdb$database');
	var_dump(ibase_fetch_assoc($q));
	
    ibase_close();
    echo "end of test\n";
?>
--EXPECT--
array(22) {
  ["ID"]=>
  int(1)
  ["ID_01"]=>
  int(2)
  ["ID_02"]=>
  int(3)
  ["ID_03"]=>
  int(4)
  ["ID_04"]=>
  int(5)
  ["ID_05"]=>
  int(6)
  ["ID_06"]=>
  int(7)
  ["ID_07"]=>
  int(8)
  ["ID_08"]=>
  int(9)
  ["ID_09"]=>
  int(10)
  ["ID_10"]=>
  int(11)
  ["CONSTANT"]=>
  int(12)
  ["CONSTANT_01"]=>
  int(13)
  ["CONSTANT_02"]=>
  int(14)
  ["CONSTANT_03"]=>
  int(15)
  ["CONSTANT_04"]=>
  int(16)
  ["CONSTANT_05"]=>
  int(17)
  ["CONSTANT_06"]=>
  int(18)
  ["CONSTANT_07"]=>
  int(19)
  ["CONSTANT_08"]=>
  int(20)
  ["CONSTANT_09"]=>
  int(21)
  ["CONSTANT_10"]=>
  int(22)
}
end of test
