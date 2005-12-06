--TEST--
Bug #26133 (ocifreedesc() segfault)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

    require dirname(__FILE__).'/connect.inc';
    require dirname(__FILE__).'/create_table.inc';

    if ($c) {
        $ora_sql = "INSERT INTO 
                               ".$schema.$table_name." (id, value) 
                         VALUES ('1','1')
                      RETURNING 
                               ROWID 
                           INTO :v_rowid ";
                      
        $statement = OCIParse($c,$ora_sql);
        $rowid = OCINewDescriptor($c,OCI_D_ROWID);
        OCIBindByName($statement,":v_rowid", $rowid,-1,OCI_B_ROWID);
        if (OCIExecute($statement)) {
            OCICommit($c);
        }
        OCIFreeStatement($statement);
        $rowid->free();
    }

    require dirname(__FILE__).'/drop_table.inc';
    
    echo "Done\n";
?>
--EXPECT--
Done
