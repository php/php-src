<?php

/* $Id$ */
/* used in tests */

srand((double)microtime()*1000000);

$test_base = dirname(__FILE__)."/ibase_test.tmp";
@unlink($test_base);

$name = tempnam(dirname(__FILE__), "CREATEDB");
$ftmp = fopen($name,"w");
fwrite($ftmp, 
"
	create database \"$test_base\";
	create table test1 (i integer, c varchar(100));
	commit;
   	insert into test1(i, c) values(1,  'test table created with isql');
	exit;
"
);
fclose($ftmp);

/* set the correct binary */
if (is_executable('isql')) {
	$cmd = 'isql';
} else {
	$cmd = '/opt/interbase/bin/isql';
}

exec("$cmd -i $name 2>&1");
@unlink($name);


function out_table($table_name)
{
	echo "--- $table_name ---\n";
	$res = ibase_query("select * from $table_name");
	$f = ibase_num_fields($res);
	while ($r = ibase_fetch_row($res)){
		for($i = 0; $i < $f; $i++)
    		echo "$r[$i]\t";
		echo "\n";
	}
	ibase_free_result($res);
	echo "---\n";
}

function out_result($result, $table_name = "")
{
	echo "--- $table_name ---\n";
	$f = ibase_num_fields($result);
	while ($r = ibase_fetch_row($result)){
		for($i = 0; $i < $f; $i++)
    		echo "$r[$i]\t";
		echo "\n";
	}
	echo "---\n";
}

/* M/D/Y H:M:S */
function rand_datetime()
{
    return sprintf("%02d/%02d/%4d %02d:%02d:%02d",
    rand()%12+1, rand()%28+1, rand()%100+1910,
    rand()%24,   rand()%60,  rand()%60);
}

/* random binary string  */
function rand_binstr($max_len)
{
    $len = rand() % $max_len;
    $s = "";
    while($len--)
        $s .= sprintf("%c", rand() % 256);
    return $s;
}

function rand_str($max_len)
{
    $len = rand() % $max_len;
    $s = "";
    while($len--)
        $s .= sprintf("%c", rand() % 26 + 65);;
    return $s;
}

function rand_number($len , $prec = -1, $sign = 1)
{
    if($prec == -1){
        $n = substr(rand() . rand(), 0, rand() % $len + 1);
        if(strlen($n) < $len)
	    	$n .= "." . substr(rand(), 0, rand() % ($len - strlen($n)) + 1);
    }elseif ($prec == 0){
        $n = substr(rand() . rand(), 0, rand() % $len + 1);
    }else{
        $n = substr(rand() . rand(), 0, rand() % ($len - $prec) + 1);
        $n .= "." . substr(rand(), 0, $prec);
    }
    if($sign && (rand() % 3 == 0))
        $n = "-" .$n;
    return $n;
}

?>
