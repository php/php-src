<?php

srand((double)microtime()*1000000);

$user = 'SYSDBA';
$password = 'masterkey';
ini_set('ibase.default_user',$user);
ini_set('ibase.default_password',$password);

/* we need just the generated name, not the file itself */
unlink($test_base = tempnam(sys_get_temp_dir(),"php_ibase_test"));

function init_db()
{
	global $test_base, $user, $password;

	$test_db = ibase_query(IBASE_CREATE,
		sprintf("CREATE SCHEMA '%s' USER '%s' PASSWORD '%s' DEFAULT CHARACTER SET %s",$test_base,
		$user, $password, ($charset = ini_get('ibase.default_charset')) ? $charset : 'NONE'));
	$tr = ibase_trans($test_db);
	ibase_query($tr,"create table test1 (i integer, c varchar(100))");
	ibase_commit_ret($tr);
	ibase_query($tr,"insert into test1(i, c) values(1, 'test table not created with isql')");
	ibase_commit($tr);
	ibase_close($test_db);
}

function cleanup_db()
{
	global $test_base;

	$r = ibase_connect($test_base);
	ibase_drop_db($r);
}

register_shutdown_function('cleanup_db');
init_db();

function out_table($table_name)
{
	echo "--- $table_name ---\n";
	$res = ibase_query("select * from $table_name");
	while ($r = ibase_fetch_row($res)) {
		echo join("\t",$r)."\t\n";
	}
	ibase_free_result($res);
	echo "---\n";
}

function out_result($result, $table_name = "")
{
	echo "--- $table_name ---\n";
	while ($r = ibase_fetch_row($result)) {
		echo join("\t",$r)."\t\n";
	}
	echo "---\n";
}

function out_result_trap_error($result, $table_name = "")
{
   echo "--- $table_name ---\n";
   while ($r = @ibase_fetch_row($result)) {
		echo join("\t",$r)."\t\n";
   }
   echo "errmsg [" . ibase_errmsg() . "]\n";
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
    while($len--) {
        $s .= sprintf("%c", rand() % 256);
    }
    return $s;
}

function rand_str($max_len)
{
    $len = rand() % $max_len;
    $s = "";
    while ($len--) {
        $s .= sprintf("%c", rand() % 26 + 65);
    }
    return $s;
}

function rand_number($len , $prec = -1, $sign = 1)
{
    if ($prec == -1) {
        $n = substr(rand() . rand(), 0, rand() % $len + 1);
        if (strlen($n) < $len) {
	    	$n .= "." . substr(rand(), 0, rand() % ($len - strlen($n)) + 1);
        }
    } else if ($prec == 0) {
        $n = substr(rand() . rand(), 0, rand() % $len + 1);
    } else if (($prec - $len) == 0) {
        $n = substr(rand() . rand(), 0, 1);
        $n .= "." . substr(rand(), 0, $prec);
    } else {
        $n = substr(rand() . rand(), 0, rand() % ($len - $prec) + 1);
        $n .= "." . substr(rand(), 0, $prec);
    }
    if ($sign && (rand() % 3 == 0)) {
        $n = "-" .$n;
    }
    return $n;
}

?>
