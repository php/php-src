<?php /* $Id$ */

if (!extension_loaded("interbase")) print "skip interbase extension not available"; 
require("interbase.inc");
if(!@ibase_connect($test_base)){
        die("skip cannot connnect");
}
?>
