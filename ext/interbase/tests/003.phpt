--TEST--
InterBase: misc sql types (may take a while)
--SKIPIF--
<?php include("skipif.inc"); ?>
--POST--
--GET--
--FILE--
<?php

    require("interbase.inc");
    
    ibase_connect($test_base);
    
    ibase_query(
    	"create table test3 (
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
    ibase_commit();

    /* if timefmt is not supported, suppress error here */
    @ibase_timefmt("%m/%d/%Y %H:%M:%S");

    for($iter = 0; $iter < 10; $iter++){
    	/* prepare data  */
    	$v_char = rand_str(1000);
    	$v_date = rand_datetime();
    	$v_decimal = rand_number(12,3);
    	$v_double  = rand_number(18);
    	$v_float   = rand_number(7);
    	$v_integer = rand_number(9,0);
    	$v_numeric = rand_number(4,2);
    	$v_smallint = rand_number(5) % 32767;
    	$v_varchar = rand_str(10000);

    	ibase_query(
    	"insert into test3 (iter, v_char,v_date,v_decimal,v_double,v_float,v_integer,v_numeric,v_smallint,v_varchar)
    	values ($iter, '$v_char','$v_date',$v_decimal,$v_double,$v_float,$v_integer,$v_numeric,$v_smallint,'$v_varchar')");
    	$sel = ibase_query("select * from test3 where iter = $iter");
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
        if($row->V_DECIMAL != $v_decimal){
            echo " DECIMAL fail\n";
            echo " in:  $v_decimal\n";
            echo " out: $row->V_DECIMAL\n";
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
        ibase_free_result($sel);
    } /* for($iter) */

    ibase_close();
    echo "end of test\n";
?>
--EXPECT--
end of test
