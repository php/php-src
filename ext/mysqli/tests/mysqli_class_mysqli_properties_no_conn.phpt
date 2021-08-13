--TEST--
Interface of the class mysqli
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require('table.inc');

    function dump_properties($mysqli) {

        printf("\nClass variables:\n");
        $variables = array_keys(get_class_vars(get_class($mysqli)));
        sort($variables);
        foreach ($variables as $k => $var) {
            try {
                printf("%s = '%s'\n", $var, var_export($mysqli->$var, true));
            } catch (Error $exception) {
                echo $exception->getMessage() . "\n";
            }
        }

        printf("\nObject variables:\n");
        $variables = array_keys(get_object_vars($mysqli));
        foreach ($variables as $k => $var) {
            try {
                printf("%s = '%s'\n", $var, var_export($mysqli->$var, true));
            } catch (Error $exception) {
                echo $exception->getMessage() . "\n";
            }
        }

        printf("\nMagic, magic properties:\n");
        try {
            mysqli_affected_rows($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->affected_rows;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        printf("mysqli->client_info = '%s'/%s\n", $mysqli->client_info, gettype($mysqli->client_info));

        printf("mysqli->client_version = '%s'/%s\n", $mysqli->client_version, gettype($mysqli->client_version));

        try {
            mysqli_errno($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->errno;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            mysqli_error($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->error;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            mysqli_field_count($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->field_count;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            mysqli_insert_id($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->insert_id;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            mysqli_sqlstate($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->sqlstate;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            mysqli_get_host_info($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->host_info;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            mysqli_info($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->info;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            mysqli_thread_id($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->thread_id;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            mysqli_get_proto_info($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->protocol_version;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            mysqli_get_server_info($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->server_info;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            mysqli_get_server_version($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->server_version;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            mysqli_warning_count($mysqli);
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }

        try {
            $mysqli->warning_count;
        } catch (Error $exception) {
            echo $exception->getMessage() . "\n";
        }


        printf("\nAccess to undefined properties:\n");
        printf("mysqli->unknown = '%s'\n", @$mysqli->unknown);

        @$mysqli->unknown = 13;
        printf("setting mysqli->unknown, @mysqli_unknown = '%s'\n", @$mysqli->unknown);

        $unknown = 'friday';
        @$mysqli->unknown = $unknown;
        printf("setting mysqli->unknown, @mysqli_unknown = '%s'\n", @$mysqli->unknown);

        printf("\nAccess hidden properties for MYSLQI_STATUS_INITIALIZED (TODO documentation):\n");
        assert(@mysqli_connect_error() === @$mysqli->connect_error);
        printf("mysqli->connect_error = '%s'/%s ('%s'/%s)\n",
            @$mysqli->connect_error, gettype(@$mysqli->connect_error),
            @mysqli_connect_error(), gettype(@mysqli_connect_error()));

        assert(@mysqli_connect_errno() === @$mysqli->connect_errno);
        printf("mysqli->connect_errno = '%s'/%s ('%s'/%s)\n",
            @$mysqli->connect_errno, gettype(@$mysqli->connect_errno),
            @mysqli_connect_errno(), gettype(@mysqli_connect_errno()));
    }

    printf("Without RS\n");
    $mysqli = @new mysqli($host, $user, $passwd . "invalid", $db, $port, $socket);
    dump_properties($mysqli);

    printf("\nWith RS\n");
    $mysqli = @new mysqli($host, $user, $passwd . "invalid", $db, $port, $socket);
    try {
        $mysqli->query("SELECT * FROM test");
    } catch (Error $exception) {
        echo $exception->getMessage() . "\n";
    }
    dump_properties($mysqli);

    print "done!";
?>
--CLEAN--
<?php require_once("clean_table.inc"); ?>
--EXPECTF--
Without RS

Class variables:
Property access is not allowed yet
client_info = '%s'
client_version = '%s'
connect_errno = '%s'
connect_error = ''%s'
mysqli object is already closed
mysqli object is already closed
Property access is not allowed yet
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed

Object variables:

Magic, magic properties:
mysqli object is already closed
Property access is not allowed yet
mysqli->client_info = '%s'/string
mysqli->client_version = '%d'/integer
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed

Access to undefined properties:
mysqli->unknown = ''
setting mysqli->unknown, @mysqli_unknown = '13'
setting mysqli->unknown, @mysqli_unknown = 'friday'

Access hidden properties for MYSLQI_STATUS_INITIALIZED (TODO documentation):
mysqli->connect_error = '%s'/%s)
mysqli->connect_errno = '%s'/integer ('%s'/integer)

With RS
mysqli object is already closed

Class variables:
Property access is not allowed yet
client_info = '%s'
client_version = '%s'
connect_errno = '%s'
connect_error = '%s'
mysqli object is already closed
mysqli object is already closed
Property access is not allowed yet
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed

Object variables:

Magic, magic properties:
mysqli object is already closed
Property access is not allowed yet
mysqli->client_info = '%s'/string
mysqli->client_version = '%d'/integer
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed
mysqli object is already closed

Access to undefined properties:
mysqli->unknown = ''
setting mysqli->unknown, @mysqli_unknown = '13'
setting mysqli->unknown, @mysqli_unknown = 'friday'

Access hidden properties for MYSLQI_STATUS_INITIALIZED (TODO documentation):
mysqli->connect_error = '%s'/%s)
mysqli->connect_errno = '%s'/integer ('%s'/integer)
done!
