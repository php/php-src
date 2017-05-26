--TEST--
Bug #73949 (leak in mysqli_fetch_object)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
require_once("connect.inc");

class cc{
    function __construct($c=null){
    }   
};
$i=mysqli_connect('p:'.$host, $user, $passwd, $db);
$res=mysqli_query($i, "SHOW STATUS LIKE 'Connections'");
$t=array(new stdClass);
while($db= mysqli_fetch_object($res,'cc',$t)){}
print "done!";
?>
--EXPECTF--
done!
