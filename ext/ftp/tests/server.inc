<?php

$socket = null;
$errno = 0;
$context = stream_context_create(array('ssl' => array('local_cert' => dirname(__FILE__).'/cert.pem')));

$socket = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND|STREAM_SERVER_LISTEN, $context);
if (!$socket) {
    echo "$errstr ($errno)\n";
    die("could not start/bind the ftp server\n");
}

$socket_name = stream_socket_get_name($socket, false);
$port = (int) substr($socket_name, strrpos($socket_name, ':') + 1);

$pid = pcntl_fork();

if ($pid) {
    function dump_and_exit($buf)
    {
        var_dump($buf);
        exit;
    }

    function anonymous()
    {
        return $GLOBALS['user'] === 'anonymous';
    }

    /* quick&dirty realpath() like function */
    function change_dir($dir)
    {
        global $cwd;

        if ($dir[0] == '/') {
            $cwd = $dir;
            return;
        }

        $cwd = "$cwd/$dir";

        do {
            $old = $cwd;
            $cwd = preg_replace('@/?[^/]+/\.\.@', '', $cwd);
        } while ($old != $cwd);

        $cwd = strtr($cwd, array('//' => '/'));
        if (!$cwd) $cwd = '/';
    }

    $s = stream_socket_accept($socket);

    if (!$s) die("Error accepting a new connection\n");

    register_shutdown_function(function() use($pid, $s) {
        fclose($s);
        pcntl_waitpid($pid, $status);
    });

    fputs($s, "220----- PHP FTP server 0.3 -----\r\n220 Service ready\r\n");
    $buf = fread($s, 2048);

    function user_auth($buf) {
        global $user, $s, $ssl, $bug37799;

        if (!empty($ssl)) {
            if ($buf !== "AUTH TLS\r\n") {
                fputs($s, "500 Syntax error, command unrecognized.\r\n");
                dump_and_exit($buf);
            }

            if (empty($bug37799)) {
                fputs($s, "234 auth type accepted\r\n");
            } else {
                fputs($s, "666 dummy\r\n");
                sleep(1);
                fputs($s, "666 bogus msg\r\n");
                exit;
            }

            if (!stream_socket_enable_crypto($s, true, STREAM_CRYPTO_METHOD_SSLv23_SERVER)) {
                die("SSLv23 handshake failed.\n");
            }

            if (!preg_match('/^PBSZ \d+\r\n$/', $buf = fread($s, 2048))) {
                fputs($s, "501 bogus data\r\n");
                dump_and_exit($buf);
            }

            fputs($s, "200 OK\r\n");
            $buf = fread($s, 2048);

            if ($buf !== "PROT P\r\n") {
                fputs($s, "504 Wrong protection.\r\n");
                dump_and_exit($buf);
            }

            fputs($s, "200 OK\r\n");

            $buf = fread($s, 2048);
        }

        if ($buf == "AUTH TLS\r\n") {
           fputs($s, "500 not supported.\r\n");
           return ;
        } else if (!preg_match('/^USER (\w+)\r\n$/', $buf, $m)) {
            fputs($s, "500 Syntax error, command unrecognized.\r\n");
            dump_and_exit($buf);
        }
        $user = $m[1];
        if ($user !== 'user' && $user !== 'anonymous') {
            fputs($s, "530 Not logged in.\r\n");
            exit;
        }

        if (anonymous()) {
            fputs($s, "230 Anonymous user logged in\r\n");

        } else {
            fputs($s, "331 User name ok, need password\r\n");

            if (!preg_match('/^PASS (\w+)\r\n$/', $buf = fread($s, 100), $m)) {
                fputs($s, "500 Syntax error, command unrecognized.\r\n");
                dump_and_exit($buf);
            }

            $pass = $m[1];
            if ($pass === 'pass') {
                fputs($s, "230 User logged in\r\n");
            } else {
                fputs($s, "530 Not logged in.\r\n");
                exit;
            }
        }
    }

    user_auth($buf);

    $cwd = '/';
    $num_bogus_cmds = 0;

    while($buf = fread($s, 4098)) {
        if (!empty($bogus)) {
            fputs($s, "502 Command not implemented (".$num_bogus_cmds++.").\r\n");

        } else if ($buf === "HELP\r\n") {
            fputs($s, "214-There is help available for the following commands:\r\n");
            fputs($s, " USER\r\n");
            fputs($s, " HELP\r\n");
            fputs($s, "214 end of list\r\n");

        } elseif ($buf === "HELP HELP\r\n") {
            fputs($s, "214 Syntax: HELP [<SP> <string>] <CRLF>\r\n");

        } elseif ($buf === "PWD\r\n") {
            fputs($s, "257 \"$cwd\" is current directory.\r\n");

        } elseif ($buf === "CDUP\r\n") {
            change_dir('..');
            fputs($s, "250 CDUP command successful.\r\n");

        } elseif ($buf === "SYST\r\n") {
            if (isset($bug27809)) {
                fputs($s, "215   OS/400 is the remote operating system. The TCP/IP version is \"V5R2M0\"\r\n");
            } elseif (isset($bug79100)) {
                // do nothing so test hits timeout
            } elseif (isset($bug80901)) {
                fputs($s, "\r\n" . str_repeat("*", 4096) . "\r\n");
            } else {
                fputs($s, "215 UNIX Type: L8.\r\n");
            }

        } elseif ($buf === "TYPE A\r\n") {
            $ascii = true;
            fputs($s, "200 OK\r\n");

        } elseif ($buf === "AUTH SSL\r\n") {
            $ascii = true;
            fputs($s, "500 not supported\r\n");

        } elseif ($buf === "TYPE I\r\n") {
            $ascii = false;
            fputs($s, "200 OK\r\n");

        } elseif ($buf === "QUIT\r\n") {
            fputs($s, "221 Bye\r\n");
            break;

        } elseif (preg_match("~^PORT (\d+),(\d+),(\d+),(\d+),(\d+),(\d+)\r\n$~", $buf, $m)) {
            $host = "$m[1].$m[2].$m[3].$m[4]";
            $port = ((int)$m[5] << 8) + (int)$m[6];
            fputs($s, "200 OK.\r\n");

        } elseif (preg_match("~^STOR ([\w/.-]+)\r\n$~", $buf, $m)) {
            fputs($s, "150 File status okay; about to open data connection\r\n");

            if(empty($pasv))
            {
                if (!$fs = stream_socket_client("tcp://$host:$port")) {
                    fputs($s, "425 Can't open data connection\r\n");
                    continue;
                }

                $data = stream_get_contents($fs);
                $orig = file_get_contents(dirname(__FILE__).'/'.$m[1]);


                if (isset($ascii) && !$ascii && $orig === $data) {
                    fputs($s, "226 Closing data Connection.\r\n");

                } elseif ((!empty($ascii) || isset($bug39583)) && $data === strtr($orig, array("\r\n" => "\n", "\r" => "\n", "\n" => "\r\n"))) {
                    fputs($s, "226 Closing data Connection.\r\n");

                } else {
                    var_dump($data);
                    var_dump($orig);
                    fputs($s, "552 Requested file action aborted.\r\n");
                }
                fclose($fs);
            }else{
                $data = file_get_contents('nm2.php');
                $orig = file_get_contents(dirname(__FILE__).'/'.$m[1]);
                if ( $orig === $data) {
                    fputs($s, "226 Closing data Connection.\r\n");

                } else {
                    var_dump($data);
                    var_dump($orig);
                    fputs($s, "552 Requested file action aborted.\r\n");
                }
            }

        } elseif (preg_match("~^APPE ([\w/.-]+)\r\n$~", $buf, $m)) {
            fputs($s, "150 File status okay; about to open data connection\r\n");

            if(empty($pasv))
            {
                if (!$fs = stream_socket_client("tcp://$host:$port")) {
                    fputs($s, "425 Can't open data connection\r\n");
                    continue;
                }

                $data = stream_get_contents($fs);
                file_put_contents(__DIR__.'/'.$m[1], $data, FILE_APPEND);
                fputs($s, "226 Closing data Connection.\r\n");
                fclose($fs);
            }else{
                $data = stream_get_contents($fs);
                file_put_contents(__DIR__.'/'.$m[1], $data, FILE_APPEND);
                fputs($s, "226 Closing data Connection.\r\n");
                fclose($fs);
            }

        }elseif (preg_match("~^CWD ([A-Za-z./]+)\r\n$~", $buf, $m)) {
            if (isset($bug77680)) {
                fputs($s, "550 Directory change to $m[1] failed: file does not exist\r\n");
                var_dump($buf);
            } else {
                change_dir($m[1]);
                fputs($s, "250 CWD command successful.\r\n");
            }

        } elseif (preg_match("~^NLST(?: ([A-Za-z./]+))?\r\n$~", $buf, $m)) {

            if (isset($m[1]) && (($m[1] === 'bogusdir') || ($m[1] === '/bogusdir'))) {
                fputs($s, "250 $m[1]: No such file or directory\r\n");
                continue;
            }

            // there are some servers that don't open the ftp-data socket if there's nothing to send
            if (isset($bug39458) && isset($m[1]) && $m[1] === 'emptydir') {
                fputs($s, "226 Transfer complete.\r\n");
                continue;
            }

            if (empty($pasv)) {
                fputs($s, "150 File status okay; about to open data connection\r\n");
                if (!$fs = stream_socket_client("tcp://$host:$port")) {
                    fputs($s, "425 Can't open data connection\r\n");
                    continue;
                }
            } else {
                fputs($s, "125 Data connection already open; transfer starting.\r\n");
                $fs=$pasvs;
            }


            if ((!empty($ssl)) && (!stream_socket_enable_crypto($pasvs, true, STREAM_CRYPTO_METHOD_SSLv23_SERVER))) {
                die("SSLv23 handshake failed.\n");
            }

            if (empty($m[1]) || $m[1] !== 'emptydir') {
                fputs($fs, "file1\r\nfile1\r\nfile\nb0rk\r\n");
            }

            fputs($s, "226 Closing data Connection.\r\n");
            fclose($fs);

        } elseif (preg_match("~^MKD ([A-Za-z./]+)\r\n$~", $buf, $m)) {
            if (isset($bug7216)) {
                fputs($s, "257 OK.\r\n");
            } else {
                if (isset($bug77680)) {
                    var_dump($buf);
                }
                fputs($s, "257 \"/path/to/ftproot$cwd$m[1]\" created.\r\n");
            }

        } elseif (preg_match('/^USER /', $buf)) {
            user_auth($buf);

        } elseif (preg_match('/^MDTM ([\w\h]+)/', $buf, $matches)) {
            switch ($matches [1]){
                case "A":
                fputs($s, "213 19980615100045.014\r\n");
                break;
                case "B":
                fputs($s, "213 19980615100045.014\r\n");
                break;
                case "C":
                fputs($s, "213 19980705132316\r\n");
                break;
                case "19990929043300 File6":
                fputs($s, "213 19991005213102\r\n");
                break;
                default :
                fputs($s, "550 No file named \"{$matches [1]}\"\r\n");
                break;
            }
        }elseif (preg_match('/^RETR ([\/]*[\w\h]+)/', $buf, $matches)) {
            if(!empty($pasv)){
                ;
            }
            else if (!$fs = stream_socket_client("tcp://$host:$port")) {
                fputs($s, "425 Can't open data connection\r\n");
                continue;
            }

            switch($matches[1]){

                case "pasv":
                    fputs($s, "150 File status okay; about to open data connection.\r\n");
                    //the data connection is handled in another forked process
                    // called from outside this while loop
                    fputs($s, "226 Closing data Connection.\r\n");
                    break;
                case "a story":
                    fputs($s, "150 File status okay; about to open data connection.\r\n");
                    fputs($fs, "For sale: baby shoes, never worn.\r\n");
                    fputs($s, "226 Closing data Connection.\r\n");
                    break;
                case "binary data":
                    fputs($s, "150 File status okay; about to open data connection.\r\n");
                    $transfer_type = $ascii? 'ASCII' : 'BINARY' ;
                    fputs($fs, $transfer_type."Foo\0Bar\r\n");
                    fputs($s, "226 Closing data Connection.\r\n");
                    break;
                case "fget":
                    fputs($s, "150 File status okay; about to open data connection.\r\n");
                    $transfer_type = $ascii? 'ASCII' : 'BINARY' ;
                    fputs($fs, $transfer_type."FooBar\r\n");
                    fputs($s, "226 Closing data Connection.\r\n");
                    break;
                case "fgetresume":
                    fputs($s, "150 File status okay; about to open data connection.\r\n");
                    $transfer_type = $ascii? 'ASCII' : 'BINARY' ;
                    fputs($fs, "Bar\r\n");
                    fputs($s, "226 Closing data Connection.\r\n");
                    break;
                case "fget_large":
                    fputs($s, "150 File status okay; about to open data connection.\r\n");
                    $transfer_type = $ascii? 'ASCII' : 'BINARY' ;
                    if ($GLOBALS['rest_pos'] == '5368709119') {
                        fputs($fs, "X");
                    } else {
                        fputs($fs, "Y");
                    }
                    fputs($s, "226 Closing data Connection.\r\n");
                    break;
                case "mediumfile":
                    fputs($s, "150 File status okay; about to open data connection.\r\n");
                    for($i = 0; $i < 150; $i++){
                        fputs($fs, "This is line $i of the test data.\n");
                    }
                    fputs($s, "226 Closing data Connection.\r\n");
                    break;
                case "/bug73457":
                    fputs($s, "150 File status okay; about to open data connection.\r\n");
                    break;
                case "gh10521":
                    // Just a side channel for getting the received file size.
                    fputs($s, "425 Can't open data connection (".$GLOBALS['rest_pos'].").\r\n");
                    break;

                default:
                    fputs($s, "550 {$matches[1]}: No such file or directory \r\n");
                    break;
            }
            if(isset($fs))
                fclose($fs);


        }elseif (preg_match('/^PASV/', $buf, $matches)) {
            $pasv=true;
            $host = "127.0.0.1";
            $i=0;

            if (empty($bug73457)) {
                if (!empty($ssl)) {
                    $soc = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND|STREAM_SERVER_LISTEN, $context);
                } else {
                    $soc = stream_socket_server("tcp://127.0.0.1:0");
                }
                if (!$soc) {
                    echo "$errstr ($errno)\n";
                    die("could not bind passive port\n");
                }

                $soc_name = stream_socket_get_name($soc, false);
                $pasv_port = (int) substr($soc_name, strrpos($soc_name, ':') + 1);
            } else {
                $pasv_port = 1234;
            }

            $p2 = $pasv_port % ((int) 1 << 8);
            $p1 = ($pasv_port-$p2)/((int) 1 << 8);
            fputs($s, "227 Entering Passive Mode. (127,0,0,1,{$p1},{$p2})\r\n");

            if (empty($bug73457)) {
                $pasvs = stream_socket_accept($soc,10);
            }

        } elseif (preg_match('/^EPSV/', $buf, $matches)) {
            fputs($s, "550 Extended passsive mode not supported.\r\n");
        } elseif (preg_match('/^SITE EXEC/', $buf, $matches)) {
            fputs($s, "200 OK\r\n");

        } elseif (preg_match('/^RMD/', $buf, $matches)) {
            fputs($s, "250 OK\r\n");

        } elseif (preg_match('/^SITE CHMOD/', $buf, $matches)) {
            fputs($s, "200 OK\r\n");

        } elseif (preg_match('/^DELE ([\w\h]+)/', $buf, $matches)) {
            if (isset($matches[1]) && in_array($matches[1], ['file1', "file\nb0rk"])){
                fputs($s, "250 Delete successful\r\n");
            } else {
                fputs($s, "550 No such file or directory\r\n");
            }
        } elseif (preg_match('/^ALLO (\d+)/', $buf, $matches)) {
            fputs($s, "200 " . $matches[1] . " bytes allocated\r\n");

        }elseif (preg_match('/^LIST www\//', $buf, $matches)) {
            if (empty($pasv)) {
                fputs($s, "150 File status okay; about to open data connection\r\n");
                if (!$fs = stream_socket_client("tcp://$host:$port")) {
                    fputs($s, "425 Can't open data connection\r\n");
                    continue;
                }
            } else {
                fputs($s, "125 Data connection already open; transfer starting.\r\n");
                $fs = $pasvs;
            }


            if ((!empty($ssl)) && (!stream_socket_enable_crypto($pasvs, true, STREAM_CRYPTO_METHOD_SSLv23_SERVER))) {
                die("SSLv23 handshake failed.\n");
            }

            fputs($fs, "file1\r\nfile1\r\nfile\nb0rk\r\n");
            fputs($s, "226 Closing data Connection.\r\n");
            fclose($fs);
            
            fputs($s, "226 Transfer complete\r\n");
        }elseif (preg_match('/^LIST no_exists\//', $buf, $matches)) {
            fputs($s, "425 Error establishing connection\r\n");

        }elseif (preg_match('/^REST (\d+)/', $buf, $matches)) {
            $GLOBALS['rest_pos'] = $matches[1];
            fputs($s, "350 OK\r\n");
        }elseif (preg_match('/^SIZE largefile/', $buf)) {
            fputs($s, "213 5368709120\r\n");
        }elseif (preg_match('/^RNFR existing_file/', $buf, $matches)) {
            fputs($s, "350 File or directory exists, ready for destination name\r\n");
        }elseif (preg_match('/^RNFR nonexisting_file/', $buf, $matches)) {
            fputs($s, "550 No such file or directory\r\n");
        }elseif (preg_match('/^RNTO nonexisting_file/', $buf, $matches)) {
            fputs($s, "250 Rename successful\r\n");
        }elseif (preg_match('/^MLSD no_exists\//', $buf, $matches)) {
            fputs($s, "425 Error establishing connection\r\n");
        }elseif (preg_match("~^MLSD(?: ([A-Za-z./]+))?\r\n$~", $buf, $m)) {

            if(isset($m[1]) && (($m[1] === 'bogusdir') || ($m[1] === '/bogusdir'))) {
                fputs($s, "250 $m[1]: No such file or directory\r\n");
                continue;
            }

            // there are some servers that don't open the ftp-data socket if there's nothing to send
            if(isset($bug39458) && isset($m[1]) && $m[1] === 'emptydir') {
                fputs($s, "226 Transfer complete.\r\n");
                continue;
            }

            if(empty($pasv)) {
                fputs($s, "150 File status okay; about to open data connection\r\n");
                if(!$fs = stream_socket_client("tcp://$host:$port")) {
                    fputs($s, "425 Can't open data connection\r\n");
                    continue;
                }
            } else {
                fputs($s, "125 Data connection already open; transfer starting.\r\n");
                $fs = $pasvs;
            }

            if((!empty($ssl)) && (!stream_socket_enable_crypto($pasvs, TRUE, STREAM_CRYPTO_METHOD_SSLv23_SERVER))) {
                die("SSLv23 handshake failed.\n");
            }

            if(empty($m[1]) || $m[1] !== 'emptydir') {
                fputs($fs, "modify=20170127230002;perm=flcdmpe;type=cdir;unique=811U4340002;UNIX.group=33;UNIX.mode=0755;UNIX.owner=33; .\r\n");
                fputs($fs, "modify=20170127230002;perm=flcdmpe;type=pdir;unique=811U4340002;UNIX.group=33;UNIX.mode=0755;UNIX.owner=33; ..\r\n");
                fputs($fs, "modify=20170126121225;perm=adfrw;size=4729;type=file;unique=811U4340CB9;UNIX.group=33;UNIX.mode=0644;UNIX.owner=33; foobar\r\n");
                fputs($fs, "fact=val=ue;empty=; path;name\r\n");
                fputs($fs, "no_space\r\n");
                fputs($fs, "no_semi pathname\r\n");
                fputs($fs, "no_eq; pathname\r\n");
            }

            fputs($s, "226 Closing data Connection.\r\n");
            fclose($fs);
        }elseif (preg_match('/^SIZE \/bug73457/', $buf)) {
            fputs($s, "213 10\r\n");
        }elseif (preg_match("/^SITE/", $buf)) {
            fputs($s, "500 Syntax error, command unrecognized.\r\n");
        }else {
            dump_and_exit($buf);
        }
    }
    exit;
}

fclose($socket);
?>
