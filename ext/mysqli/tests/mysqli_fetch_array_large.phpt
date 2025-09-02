--TEST--
mysqli_fetch_array() - large packages (to test compression)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
require_once 'skipifconnectfailure.inc';
?>
--CONFLICTS--
all
--INI--
memory_limit=-1
--FILE--
<?php
    require_once 'connect.inc';

    function mysqli_fetch_array_large($offset, $link, $package_size) {

        /* we are aiming for maximum compression to test MYSQLI_CLIENT_COMPRESS */
        $random_char = str_repeat('a', 255);
        $sql = "INSERT INTO test(label) VALUES ";

        while (strlen($sql) < ($package_size - 259))
            $sql .= sprintf("('%s'), ", $random_char);

        $sql = substr($sql, 0, -2);
        $len = strlen($sql);
        assert($len < $package_size);

        if (!@mysqli_query($link, $sql)) {
            if (1153 == mysqli_errno($link) || 2006 == mysqli_errno($link) || stristr(mysqli_error($link), 'max_allowed_packet'))
                /*
                    myslqnd - [1153] Got a packet bigger than 'max_allowed_packet' bytes
                    libmysql -[2006] MySQL server has gone away
                */
                return false;

            printf("[%03d + 1] len = %d, [%d] %s\n", $offset, $len, mysqli_errno($link), mysqli_error($link));
            return false;
        }

        /* buffered result set - let's hope we do not run into PHP memory limit... */
        if (!$res = mysqli_query($link, "SELECT id, label FROM test")) {
            printf("[%03d + 2] len = %d, [%d] %s\n", $offset, $len, mysqli_errno($link), mysqli_error($link));
            return false;
        }

        while ($row = mysqli_fetch_assoc($res)) {
            if ($row['label'] != $random_char) {
                printf("[%03d + 3] Wrong results - expecting '%s' got '%s', len = %d, [%d] %s\n",
                    $offset, $random_char, $row['label'], $len, mysqli_errno($link), mysqli_error($link));
                return false;
            }
        }
        mysqli_free_result($res);

        if (!$stmt = mysqli_prepare($link, "SELECT id, label FROM test")) {
            printf("[%03d + 4] len = %d, [%d] %s\n", $offset, $len, mysqli_errno($link), mysqli_error($link));
            return false;
        }

        /* unbuffered result set */
        if (!mysqli_stmt_execute($stmt)) {
            printf("[%03d + 5] len = %d, [%d] %s, [%d] %s\n", $offset, $len, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt), mysqli_errno($link), mysqli_error($link));
            return false;
        }

        $id = $label = NULL;
        if (!mysqli_stmt_bind_result($stmt, $id, $label)) {
            printf("[%03d + 6] len = %d, [%d] %s, [%d] %s\n", $offset, $len, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt), mysqli_errno($link), mysqli_error($link));
            return false;
        }

        while (mysqli_stmt_fetch($stmt)) {
            if ($label != $random_char) {
                printf("[%03d + 7] Wrong results - expecting '%s' got '%s', len = %d, [%d] %s\n",
                    $offset, $random_char, $label, $len, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
                return false;
            }
        }

        mysqli_stmt_free_result($stmt);
        mysqli_stmt_close($stmt);

        return true;
    }

    function parse_memory_limit($limit) {

        $val = trim($limit);
        $last = strtolower($val[strlen($val)-1]);

        switch($last) {
                // The 'G' modifier is available since PHP 5.1.0
                case 'g':
                    $val *= 1024;
                case 'm':
                    $val *= 1024;
                case 'k':
                    $val *= 1024;
                default:
                    break;
        }
            return $val;
    }


    function test_fetch($host, $user, $passwd, $db, $port, $socket, $engine, $flags = null) {

        $link = mysqli_init();
        if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, $flags)) {
            printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
            $host, $user, $db, $port, $socket);
            return false;
        }

        if (!mysqli_query($link, "DROP TABLE IF EXISTS test") ||
            !mysqli_query($link, sprintf("CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, label VARCHAR(255)) ENGINE = %s", $engine))) {
            printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
            return false;
        }

        $package_size = 524288;
        $offset = 3;
        $limit = (ini_get('memory_limit') > 0) ? parse_memory_limit(ini_get('memory_limit')) : pow(2, 32);

        /* try to respect php.ini but make run time a soft limit */
        $max_runtime = (ini_get('max_execution_time') > 0) ? ini_get('max_execution_time') : 30;
        set_time_limit(0);

        do {
            if ($package_size > $limit) {
                printf("stop: memory limit - %s vs. %s\n", $package_size, $limit);
                break;
            }

            $start = microtime(true);
            if (!mysqli_fetch_array_large($offset++, $link, $package_size)) {
                printf("stop: packet size - %d\n", $package_size);
                break;
            }

            $duration = microtime(true) - $start;
            $max_runtime -= $duration;
            if ($max_runtime < ($duration * 3)) {
                /* likely the next iteration will not be within max_execution_time */
                printf("stop: time limit - %2.2fs\n", $max_runtime);
                break;
            }

            $package_size += $package_size;

        } while (true);


        mysqli_close($link);
        return true;
    }


    test_fetch($host, $user, $passwd, $db, $port, $socket, $engine, null);
    test_fetch($host, $user, $passwd, $db, $port, $socket, $engine, MYSQLI_CLIENT_COMPRESS);
    print "done!";
?>
--CLEAN--
<?php
	require_once 'clean_table.inc';
?>
--EXPECTF--
stop: %s
stop: %s
done!
