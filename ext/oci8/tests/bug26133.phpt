--TEST--
Bug #26133 (ocifreedesc() segfault)
--SKIPIF--
<?php
    require 'skipif.inc'; 
?>
--FILE--
<?php
    require 'connect.inc';
    require 'create_table.inc';
   
    if ($connection) {
        $ora_sql = "INSERT INTO 
                               ".$schema."php_test_table (id, value) 
                         VALUES ('1','1')
                      RETURNING 
                               ROWID 
                           INTO :v_rowid ";
                      
        $statement = OCIParse($connection,$ora_sql);
        $rowid = OCINewDescriptor($connection,OCI_D_ROWID);
        OCIBindByName($statement,":v_rowid", $rowid,-1,OCI_B_ROWID);
        if (OCIExecute($statement)) {
            OCICommit($connection);
        }
        OCIFreeStatement($statement);
        $rowid->free();
    }

    require 'drop_table.inc';
    
    echo "Done\n";
?>
--EXPECTF--
Done

