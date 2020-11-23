--TEST--
mysqli_fetch_assoc() - utf8
--SKIPIF--
<?php
    require_once('skipif.inc');
    require_once('skipifconnectfailure.inc');
    require_once("connect.inc");

    if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
        die("skip Cannot connect to server to check charsets");

    if (!$res = mysqli_query($link, "SHOW CHARACTER SET LIKE 'UTF8'"))
        die("skip Cannot run SHOW CHARACTER SET to check charsets");

    if (!$tmp = mysqli_fetch_assoc($res))
        die("skip Looks like UTF8 is not available on the server");

    if (strtolower($tmp['Charset']) !== 'utf8')
        die("skip Not sure if UTF8 is available, canceling the test");

    mysqli_free_result($res);

    if (!$res = mysqli_query($link, "SHOW CHARACTER SET LIKE 'UCS2'"))
        die("skip Cannot run SHOW CHARACTER SET to check charsets");

    if (!$tmp = mysqli_fetch_assoc($res))
        die("skip Looks like UCS2 is not available on the server");

    if (strtolower($tmp['Charset']) !== 'ucs2')
        die("skip Not sure if UCS2 is available, canceling the test");

    mysqli_free_result($res);
    mysqli_close($link);
?>
--FILE--
<?php
    require('table.inc');

    /* some cyrillic (utf8) comes here */
    if (!$res = mysqli_query($link, "SET NAMES UTF8")) {
        printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    if (!$res = mysqli_query($link, "SELECT 1 AS 'Андрей Христов', 2 AS 'Улф Вендел', 3 AS 'Георг Рихтер'")) {
        printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }
    print "[003]\n";
    var_dump(mysqli_fetch_assoc($res));
    mysqli_free_result($res);

    if (!$res = mysqli_query($link, "CREATE TABLE автори_на_mysqlnd (id integer not null auto_increment primary key, име varchar(20) character set ucs2, фамилия varchar(20) character set utf8)")) {
        printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }
    if (!$res = mysqli_query($link, "INSERT INTO автори_на_mysqlnd (име, фамилия) VALUES ('Андрей', 'Христов'), ('Георг', 'Рихтер'), ('Улф','Вендел')")) {
        printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }
    if (!$res = mysqli_query($link, "INSERT INTO автори_на_mysqlnd (име, фамилия) VALUES ('Andrey', 'Hristov'), ('Georg', 'Richter'), ('Ulf','Wendel')")) {
        printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }
    if (!$res = mysqli_query($link, "INSERT INTO автори_на_mysqlnd (име, фамилия) VALUES ('安德烈', 'Hristov'), ('格奥尔', 'Richter'), ('乌尔夫','Wendel')")) {
        printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    if (!$res = mysqli_query($link, "SELECT id, име, фамилия FROM автори_на_mysqlnd ORDER BY фамилия, име")) {
        printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }
    print "[009]\n";
    while ($row = mysqli_fetch_assoc($res)) {
        var_dump($row);
    }
    mysqli_free_result($res);

    if (!$res = mysqli_query($link, "DROP TABLE автори_на_mysqlnd")) {
        printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    }

    mysqli_close($link);
    print "done!";
?>
--EXPECTF--
[003]
array(3) {
  ["Андрей Христов"]=>
  string(%r[1|3]%r) "1"
  ["Улф Вендел"]=>
  string(1) "2"
  ["Георг Рихтер"]=>
  string(1) "3"
}
[009]
array(3) {
  ["id"]=>
  string(1) "4"
  ["име"]=>
  string(6) "Andrey"
  ["фамилия"]=>
  string(7) "Hristov"
}
array(3) {
  ["id"]=>
  string(1) "7"
  ["име"]=>
  string(9) "安德烈"
  ["фамилия"]=>
  string(7) "Hristov"
}
array(3) {
  ["id"]=>
  string(1) "5"
  ["име"]=>
  string(5) "Georg"
  ["фамилия"]=>
  string(7) "Richter"
}
array(3) {
  ["id"]=>
  string(1) "8"
  ["име"]=>
  string(9) "格奥尔"
  ["фамилия"]=>
  string(7) "Richter"
}
array(3) {
  ["id"]=>
  string(1) "6"
  ["име"]=>
  string(3) "Ulf"
  ["фамилия"]=>
  string(6) "Wendel"
}
array(3) {
  ["id"]=>
  string(1) "9"
  ["име"]=>
  string(9) "乌尔夫"
  ["фамилия"]=>
  string(6) "Wendel"
}
array(3) {
  ["id"]=>
  string(1) "3"
  ["име"]=>
  string(6) "Улф"
  ["фамилия"]=>
  string(12) "Вендел"
}
array(3) {
  ["id"]=>
  string(1) "2"
  ["име"]=>
  string(10) "Георг"
  ["фамилия"]=>
  string(12) "Рихтер"
}
array(3) {
  ["id"]=>
  string(1) "1"
  ["име"]=>
  string(12) "Андрей"
  ["фамилия"]=>
  string(14) "Христов"
}
done!
