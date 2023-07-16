--TEST--
Bug #71998 Function pg_insert does not insert when column type = inet
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
// Kudos for the IP regex to
// http://stackoverflow.com/a/17871737/3358424

include('config.inc');

$db = pg_connect($conn_str);

pg_query($db, "CREATE TABLE tmp_statistics (id integer NOT NULL, remote_addr inet);");

$ips = array(
    /* IPv4*/
    "127.0.0.1",
    "10.0.0.1",
    "192.168.1.1",
    "0.0.0.0",
    "255.255.255.255",
    "192.168.1.35/24",

    /* IPv6 */
    "::1",
    "::10.2.3.4",
    "::ffff:10.4.3.2",
    "1:2:3:4:5:6:7:8",
    "::ffff:10.0.0.1",
    "::ffff:1.2.3.4",
    "::ffff:0.0.0.0",
    "1:2:3:4:5:6:77:88",
    "::ffff:255.255.255.255",
    "fe08::7:8",
    "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
    "::5:aef1:ffff/128",
    "2001:4f8:3:ba::/112",

);

$bad = array(
    /* bad */
    "256.257.258.259",
    "fe08::7:8interface",
    "schnitzel",
    "10002.3.4",
    "1.2.3.4.5",
    "256.0.0.0",
    "260.0.0.0",
);

$ips = array_merge($ips, $bad);
$i = 0;
$errors = 0;
foreach ($ips as $ip) {
    $data = array("id" => ++$i, "remote_addr" => $ip);
    $r = true;
    try {
    	@pg_insert($db, 'tmp_statistics', $data);
    } catch (\ValueError $e) {
        echo $e->getMessage() . PHP_EOL;
        $r = false;
    }

    if (!$r && in_array($ip, $bad)) {
        $errors++;
        //echo pg_last_error($db);
    }

    //pg_query($db, "INSERT INTO tmp_statistics (id, remote_addr) VALUES (2, '127.0.0.1')"); // OK, record inserted
}


$r = pg_query($db, "SELECT * FROM tmp_statistics");
while (false != ($row = pg_fetch_row($r))) {
    var_dump($row);
}
echo $errors, " errors caught\n";

pg_query($db, "DROP TABLE tmp_statistics");
pg_close($db);

?>
--EXPECT--
pg_insert(): Field "remote_addr" must be a valid IPv4 or IPv6 address string, "256.257.258.259" given
pg_insert(): Field "remote_addr" must be a valid IPv4 or IPv6 address string, "fe08::7:8interface" given
pg_insert(): Field "remote_addr" must be a valid IPv4 or IPv6 address string, "schnitzel" given
pg_insert(): Field "remote_addr" must be a valid IPv4 or IPv6 address string, "10002.3.4" given
pg_insert(): Field "remote_addr" must be a valid IPv4 or IPv6 address string, "1.2.3.4.5" given
pg_insert(): Field "remote_addr" must be a valid IPv4 or IPv6 address string, "256.0.0.0" given
pg_insert(): Field "remote_addr" must be a valid IPv4 or IPv6 address string, "260.0.0.0" given
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(9) "127.0.0.1"
}
array(2) {
  [0]=>
  string(1) "2"
  [1]=>
  string(8) "10.0.0.1"
}
array(2) {
  [0]=>
  string(1) "3"
  [1]=>
  string(11) "192.168.1.1"
}
array(2) {
  [0]=>
  string(1) "4"
  [1]=>
  string(7) "0.0.0.0"
}
array(2) {
  [0]=>
  string(1) "5"
  [1]=>
  string(15) "255.255.255.255"
}
array(2) {
  [0]=>
  string(1) "6"
  [1]=>
  string(15) "192.168.1.35/24"
}
array(2) {
  [0]=>
  string(1) "7"
  [1]=>
  string(3) "::1"
}
array(2) {
  [0]=>
  string(1) "8"
  [1]=>
  string(10) "::10.2.3.4"
}
array(2) {
  [0]=>
  string(1) "9"
  [1]=>
  string(15) "::ffff:10.4.3.2"
}
array(2) {
  [0]=>
  string(2) "10"
  [1]=>
  string(15) "1:2:3:4:5:6:7:8"
}
array(2) {
  [0]=>
  string(2) "11"
  [1]=>
  string(15) "::ffff:10.0.0.1"
}
array(2) {
  [0]=>
  string(2) "12"
  [1]=>
  string(14) "::ffff:1.2.3.4"
}
array(2) {
  [0]=>
  string(2) "13"
  [1]=>
  string(14) "::ffff:0.0.0.0"
}
array(2) {
  [0]=>
  string(2) "14"
  [1]=>
  string(17) "1:2:3:4:5:6:77:88"
}
array(2) {
  [0]=>
  string(2) "15"
  [1]=>
  string(22) "::ffff:255.255.255.255"
}
array(2) {
  [0]=>
  string(2) "16"
  [1]=>
  string(9) "fe08::7:8"
}
array(2) {
  [0]=>
  string(2) "17"
  [1]=>
  string(39) "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"
}
array(2) {
  [0]=>
  string(2) "18"
  [1]=>
  string(13) "::5:aef1:ffff"
}
array(2) {
  [0]=>
  string(2) "19"
  [1]=>
  string(19) "2001:4f8:3:ba::/112"
}
7 errors caught
