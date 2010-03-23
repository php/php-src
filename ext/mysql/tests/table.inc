<?PHP
require_once 'connect.inc';

// connect + select_db
if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("Cannot connect to the server using host=%s/%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
 	  $host, $myhost, $user, $db, $port, $socket);
	exit(1);
}

if (!mysql_query('DROP TABLE IF EXISTS test', $link)) {
	printf("Failed to drop old test table: [%d] %s\n", mysql_errno($link), mysql_error($link));
	exit(1);
}

if (!mysql_query('CREATE TABLE test(id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE=' . $engine, $link)) {
	printf("Failed to create test table: [%d] %s\n", mysql_errno($link), mysql_error($link));
	exit(1);
}

if (!mysql_query("INSERT INTO test(id, label) VALUES (1, 'a'), (2, 'b'), (3, 'c'), (4, 'd'), (5, 'e'), (6, 'f')", $link)) {
	printf("[%d] %s\n",  mysql_errno($link), mysql_error($link));
}
?>
