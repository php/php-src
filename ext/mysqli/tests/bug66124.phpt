--TEST--
Bug #66124 (mysqli under mysqlnd loses precision when bind_param with 'i')
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('connect.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
$table_drop = "DROP TABLE IF EXISTS `bug66124`";
$table_create = "CREATE TABLE `bug66124` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8";

$table_insert = "INSERT INTO `bug66124` SET `id`=?";
$table_select = "SELECT * FROM `bug66124`";
$table_delete = "DELETE FROM `bug66124`";
$id = '1311200011005001566';


require_once('connect.inc');

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    printf("Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
        $host, $user, $db, $port, $socket);
    exit(1);
}

$link->query($table_drop);
$link->query($table_create);

$stmt = $link->prepare($table_insert);
if (!$stmt) {
    printf("Can't prepare\n");
    exit(1);
}

echo "Using 'i':\n";
$stmt->bind_param('i', $id);

if ($stmt->execute()){
    echo "insert id:{$id}=>{$stmt->insert_id}\n";
} else {
    printf("Can't execute\n");
    exit(1);
}


$result = $link->query($table_select);

if ($result){
    while ($row = $result->fetch_assoc()) {
        echo "fetch  id:{$row['id']}\n";
    }
} else {
    printf("Can't select\n");
    exit(1);
}

$stmt->close();

$link->query($table_drop);
$link->query($table_create);


$stmt = $link->prepare($table_insert);
$stmt->bind_param('s', $id);

echo "Using 's':\n";

if ($stmt->execute()){
    echo "insert id:{$id}\n";
} else{
    printf("Can't execute\n");
    exit(1);
}

$result = $link->query($table_select);

if ($result){
    while ($row = $result->fetch_assoc()) {
        echo "fetch  id:{$row['id']}\n";
    }
} else {
    printf("Can't select\n");
    exit(1);
}

$link->close();
?>
done
--EXPECT--
Using 'i':
insert id:1311200011005001566=>1311200011005001566
fetch  id:1311200011005001566
Using 's':
insert id:1311200011005001566
fetch  id:1311200011005001566
done
