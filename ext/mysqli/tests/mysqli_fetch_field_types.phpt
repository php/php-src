--TEST--
mysqli_fetch_field() - data types/field->type
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require('table.inc');

    function mysqli_field_datatypes($link, $sql_type, $php_value, $php_type, $datatypes, $default_charset="latin1") {
        if (!mysqli_query($link, "DROP TABLE IF EXISTS test")) {
            printf("[001] %s, [%d] %s\n", $sql_type,
                mysqli_errno($link), mysqli_error($link));
            return false;
        }

        $create = sprintf("CREATE TABLE test(id %s) DEFAULT CHARSET %s", $sql_type, $default_charset);
        if (!mysqli_query($link, $create)) {
            printf("[002] '%s' - '%s', [%d] %s\n", $sql_type, $create,
                mysqli_errno($link), mysqli_error($link));
            return false;
        }

        if (is_string($php_value))
            $sql = sprintf("INSERT INTO test(id) VALUES ('%s')", $php_value);
        else
            $sql = sprintf("INSERT INTO test(id) VALUES (%s)", $php_value);

        if (!mysqli_query($link, $sql)) {
            printf("[003] '%s' - '%s' - '%s', [%d] %s\n", $sql_type, $create, $sql,
                mysqli_errno($link), mysqli_error($link));
            return false;
        }

        if (!$res = mysqli_query($link, 'SELECT id FROM test')) {
            printf("[004] %s, [%d] %s\n", $sql_type,
                mysqli_errno($link), mysqli_error($link));
            return false;
        }

        if (!is_object($field = mysqli_fetch_field($res))) {
            printf("[004] %s, expecting object got %s, [%d] %s\n", $sql_type,
                gettype($field),
                mysqli_errno($link), mysqli_error($link));
            return false;
        }

        if ($field->type != $php_type) {
            $code_name = 'unknown';
            foreach ($datatypes as $k => $v) {
                if ($k == $field->type) {
                    $code_name = (is_array($v)) ? $v[0] : $v;
                    break;
                }
            }
            printf("[006] Expecting %d for %s got code %d for %s\n",
                $php_type, $sql_type, $field->type, $code_name);
            return false;
        }

        return true;
    }

    $datatypes = array(
        MYSQLI_TYPE_TINY => array('TINYINT', 5),
        MYSQLI_TYPE_SHORT => array('SMALLINT', 10),
        MYSQLI_TYPE_LONG => 'MYSQLI_TYPE_LONG - TODO add testing',
        MYSQLI_TYPE_FLOAT => array('FLOAT', '1.3'),
        MYSQLI_TYPE_DOUBLE => array('DOUBLE', '1.4'),
        MYSQLI_TYPE_TIMESTAMP => array('TIMESTAMP', '2007-08-20 18:34:00'),
        MYSQLI_TYPE_LONGLONG => array('BIGINT', 100),
        MYSQLI_TYPE_INT24	=> array('MEDIUMINT', 10),
        MYSQLI_TYPE_DATE => array('DATE', '2007-08-20'),
        MYSQLI_TYPE_TIME => array('TIME', '18:41:38'),
        MYSQLI_TYPE_DATETIME => array('DATETIME', '2007-08-20 18:42:01'),
        MYSQLI_TYPE_YEAR => array('YEAR', '2007'),
        MYSQLI_TYPE_ENUM => array('ENUM("everything", "is", "just", "wonderful")', 'is'),
        // MYSQLI_TYPE_SET	=> array('SET("I", "smash", "the")', 'I,smash,the'), - string
        // MYSQLI_TYPE_TINY_BLOB => array("TINYBLOB", "I got a tiny blog"), - blob
        // MYSQLI_TYPE_MEDIUM_BLOB => array("MEDIUMBLOB", "No blob for masses"), - blob
        // MYSQLI_TYPE_LONG_BLOB => array("LONGBLOB", "Small is beautiful?"), - blob
        MYSQLI_TYPE_BLOB => array("LONGBLOB", 'MySQL does not report proper type. Use Length to distinct BLOB types'),
        MYSQLI_TYPE_BLOB => array("MEDIUMBLOB", 'MySQL does not report proper type. Use Length to distinct BLOB types'),
        MYSQLI_TYPE_BLOB => array("TINYBLOB", 'MySQL does not report proper type. Use Length to distinct BLOB types'),
        MYSQLI_TYPE_BLOB => array("BLOB", 'silly'),
        MYSQLI_TYPE_VAR_STRING => array("VARCHAR(32768)", 'varchar'),
        MYSQLI_TYPE_STRING => 'MYSQLI_TYPE_STRING - TODO add testing',
        MYSQLI_TYPE_STRING => array('CHAR(1)', 'a'),
        MYSQLI_TYPE_STRING => array("SET('I', 'smash', 'the')", 'smash'),
        MYSQLI_TYPE_NULL => 'MYSQLI_TYPE_NULL - TODO add testing',
        MYSQLI_TYPE_NEWDATE => 'MYSQLI_TYPE_NEWDATE - TODO add testing',
        MYSQLI_TYPE_INTERVAL => 'MYSQLI_TYPE_INTERVAL - TODO add testing',
        MYSQLI_TYPE_GEOMETRY => 'MYSQLI_TYPE_GEOMETRY - TODO add testing',
    );

    if ($IS_MYSQLND) {
        $version = 50007 + 1;
    } else {
        $version = mysqli_get_client_version();
    }

    if ($version > 50002) {
        $datatypes[MYSQLI_TYPE_NEWDECIMAL] = array('DECIMAL', '1.1');
        $datatypes[MYSQLI_TYPE_BIT] = array('BIT', 0);
    } else {
        $datatypes[MYSQLI_TYPE_DECIMAL] = array('DECIMAL', '1.1');
    }

    foreach ($datatypes as $php_type => $datatype) {
        if (is_array($datatype))
            mysqli_field_datatypes($link, $datatype[0], $datatype[1], $php_type, $datatypes);
    }

    mysqli_close($link);

    print "done!";
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
done!
