--TEST--
Bug #73949 (leak in mysqli_fetch_object)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

class cc{
    function __construct($c=null){
    }
};
$i=mysqli_connect('p:'.$host, $user, $passwd, $db, $port);
$res=mysqli_query($i, "SHOW STATUS LIKE 'Connections'");
$t=array(new stdClass);
while($db= mysqli_fetch_object($res,'cc',$t)){}
print "done!";
?>
--EXPECT--
done!
