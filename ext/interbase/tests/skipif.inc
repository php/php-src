<?php

/* $Id$ */
/* used in tests */

if (!extension_loaded("interbase")) print "skip"; 
if (!is_executable("isql") && !is_executable("/opt/interbase/bin/isql")) print "skip";

?>
