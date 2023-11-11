<?PHP
require_once 'connect.inc';

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
    printf("Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
        $host, $user, $db, $port, $socket);
    exit(1);
}

if (!mysqli_query($link, 'DROP TABLE IF EXISTS test')) {
    printf("Failed to drop old test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    exit(1);
}

if (!mysqli_query($link, 'SET SESSION sql_mode=\'\'')) {
    printf("Failed to drop old test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    exit(1);
}

if (!mysqli_query($link, 'CREATE TABLE test(id INT DEFAULT 0, label CHAR(1), PRIMARY KEY(id)) ENGINE=' . $engine)) {
    printf("Failed to create test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    exit(1);
}

if (!mysqli_query($link, "INSERT INTO test(id, label) VALUES (1, 'a'), (2, 'b'), (3, 'c'), (4, 'd'), (5, 'e'), (6, 'f')")) {
    printf("[%d] %s\n",  mysqli_errno($link), mysqli_error($link));
}
?>
