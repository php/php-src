<?php

// ------------------------------------------------------------------------- //

function cleanall($dir = null)
{
    if ($dir !== null) {
        $statedir = $dir;
    } else {
        $statedir = $GLOBALS['statedir'];
    }
    if (file_exists($statedir)) {
        $dp = opendir($statedir);
        while ($ent = readdir($dp)) {
            if (in_array($ent, array('.', '..'))) {
                continue;
            }
            if (is_dir($statedir . DIRECTORY_SEPARATOR . $ent)) {
                cleanall($statedir . DIRECTORY_SEPARATOR . $ent);
                continue;
            }
            unlink($statedir . DIRECTORY_SEPARATOR . $ent);
        }
        closedir($dp);
        rmdir($statedir);
    }
}


function dumpall(&$reg)
{
    print "dumping registry...\n";
    $infos = $reg->allPackageInfo();
    foreach ($infos as $channel => $info) {
        echo "channel $channel:\n";
        foreach ($info as $pkg) {
            print $pkg["name"] . ":";
            unset($pkg["name"]);
            foreach ($pkg as $k => $v) {
                if ($k == '_lastmodified') continue;
                if (is_array($v) && $k == 'filelist') {
                    print " $k=array(";
                    $i = 0;
                    foreach ($v as $k2 => $v2) {
                        if ($i++ > 0) print ",";
                        print "{$k2}[";
                        $j = 0;
                        foreach ($v2 as $k3 => $v3) {
                            if ($j++ > 0) print ",";
                            print "$k3=$v3";
                        }
                        print "]";
                    }
                    print ")";
                } else {
                    print " $k=\"$v\"";
                }
            }
            print "\n";
        }
    }
    print "dump done\n";
}

require_once 'PEAR/ErrorStack.php';
function dump_error($err)
{
    echo "caught ErrorStack error:\n";
    echo "message: ". $err['message'] . "\n";
    echo "code: ". $err['code'] . "\n";
}
PEAR_ErrorStack::setDefaultCallback('dump_error');
register_shutdown_function('cleanall');
?>