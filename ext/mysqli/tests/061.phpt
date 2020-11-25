--TEST--
local infile handler
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
if (!function_exists('mysqli_set_local_infile_handler'))
    die("skip - function not available.");

$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
if (!$link)
    die(sprintf("skip Can't connect [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

include_once("local_infile_tools.inc");
if ($msg = check_local_infile_support($link, $engine))
    die(sprintf("skip %s, [%d] %s", $msg, $link->errno, $link->error));

mysqli_close($link);
?>
--INI--
mysqli.allow_local_infile=1
--FILE--
<?php
    require_once("connect.inc");

    function my_read($fp, &$buffer, $buflen, &$error) {
        $buffer = strrev(fread($fp, $buflen));
        return(strlen($buffer));
    }

    /*** test mysqli_connect 127.0.0.1 ***/
    $link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

    /* create temporary file */
    $filename = __DIR__ . "061.csv";
    $fp = fopen($filename, "w");
    fwrite($fp, "foo;bar");
    fclose($fp);

    if (!mysqli_query($link,"DROP TABLE IF EXISTS t_061"))
        printf("Cannot drop table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
    if (!mysqli_query($link,"CREATE TABLE t_061 (c1 varchar(10), c2 varchar(10))"))
        printf("Cannot create table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if (!mysqli_query($link, sprintf("LOAD DATA LOCAL INFILE '%s' INTO TABLE t_061 FIELDS TERMINATED BY ';'", mysqli_real_escape_string($link, $filename))))
        printf("Cannot load data: [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    mysqli_set_local_infile_handler($link, "my_read");
    if (!mysqli_query($link, sprintf("LOAD DATA LOCAL INFILE '%s' INTO TABLE t_061 FIELDS TERMINATED BY ';'", mysqli_real_escape_string($link, $filename))))
        printf("Cannot load data using infile handler: [%d] %s\n", mysqli_errno($link), mysqli_error($link));

    if ($result = mysqli_query($link, "SELECT c1,c2 FROM t_061")) {
        while (($row = mysqli_fetch_row($result))) {
            printf("%s-%s\n", $row[0], $row[1]);
            printf("%s-%s\n", gettype($row[0]), gettype($row[1]));
        }
        mysqli_free_result($result);
    }

    mysqli_close($link);
    unlink($filename);
    print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS t_061"))
    printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECT--
foo-bar
string-string
rab-oof
string-string
done!
