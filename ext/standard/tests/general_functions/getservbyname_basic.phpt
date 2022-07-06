--TEST--
Test function getservbyname()
--CREDITS--
Italian PHP TestFest 2009 Cesena 19-20-21 june
Fabio Fabbrucci (fabbrucci@grupporetina.com)
Michele Orselli (mo@ideato.it)
Simone Gentili (sensorario@gmail.com)
--SKIPIF--
<?php
if(in_array(PHP_OS_FAMILY, ['BSD', 'Darwin', 'Solaris', 'Linux'])){
    if (!file_exists("/etc/services")) die("skip reason: missing /etc/services");
}
if (getenv('SKIP_MSAN')) die('skip msan missing interceptor for getservbyname()');
?>
--FILE--
<?php

    $services = array('http', 'ftp', 'ssh', 'telnet', 'imap', 'smtp', 'nicname', 'gopher', 'finger', 'pop3', 'www');

    foreach ($services as $service) {
            $port = getservbyname($service, 'tcp');
            var_dump($port);
    }


?>
--EXPECTF--
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
int(%d)
