--TEST--
InterBase: transactions
--SKIPIF--
<?php include("skipif.inc"); ?>
--POST--
--GET--
--FILE--
<?php

    require("interbase.inc");
    
    ibase_connect($test_base);

    @ibase_query("create table test5 (i integer)");
    @ibase_query("delete from test5");
    ibase_close();


    echo "default transaction:\n";
    
/*
Difference between default and other transactions:
default commited when you call  ibase_close().
Other transaction doing rollback.

If you not open default transaction with
ibase_trans, default transaction open
when you call ibase_query(), ibase_prepare(),
ibase_blob_create(), ibase_blob_import()  first time.
*/

/*
simple default transaction test without ibase_trans()
*/
    
    ibase_connect($test_base);

    echo "empty table\n";

	/*  out_table call ibase_query()
      and ibase_query() start default transaction */
    out_table("test5");   

    /* in default transaction context */
    ibase_query("insert into test5 (i) values (1)");

    echo "one row\n";
    out_table("test5");

    ibase_rollback(); /* default rolled */

    echo "after rollback table empty again\n";
    out_table("test5");  /* started new default transaction */
    
    ibase_query("insert into test5 (i) values (2)");

    ibase_close(); /* commit here! */
    
    ibase_connect($test_base);
    
    echo "one row\n";
    out_table("test5");
    ibase_close();

/*
default transaction on default link
First open transaction on link will be default.
$tr_def_l1 may be ommited. All queryes without link and trans
parameters run in this context
*/
    
    $link_def = ibase_connect($test_base);
    
    $tr_def_l1 = ibase_trans(IBASE_READ); /* here transaction start */
    
    /* all default */
	$res = ibase_query("select * from test5");
    
    echo "one row\n";
    out_result($res,"test5");

    ibase_free_result($res);

    /* specify transaction context...  */
	$res = ibase_query($tr_def_l1, "select * from test5");
    
    echo "one row... again.\n";
    out_result($res,"test5");

    ibase_free_result($res);
    
    /* specify default transaction on link  */
	$res = ibase_query($link_def, "select * from test5");
    
    echo "one row.\n";
    out_result($res,"test5");

    ibase_free_result($res);

    ibase_rollback($link_def); /* just for example */

    ibase_close();
    
/*
three transaction on default link
*/
    ibase_connect($test_base);
    
	$tr_1 = ibase_trans();  /* this default transaction also */
	$tr_2 = ibase_trans(IBASE_READ);
	$tr_3 = ibase_trans(IBASE_READ+IBASE_COMMITTED);
    
	$res = ibase_query("select * from test5");
    
    echo "one row\n";
    out_result($res,"test5");

    ibase_free_result($res);

    /* insert in first transaction context...  */
    /* as default */
    ibase_query("insert into test5 (i) values (3)");
    /* specify context */
    ibase_query($tr_1, "insert into test5 (i) values (4)");
    
	$res = ibase_query("select * from test5");
    
    echo "three rows\n";
    out_result($res,"test5");

    ibase_free_result($res);
    
	$res = ibase_query($tr_1, "select * from test5");
    
    echo "three rows again\n";
    out_result($res,"test5");

    ibase_free_result($res);
    
    ibase_commit($tr_1);

	$res = ibase_query($tr_2, "select * from test5");
    
    echo "one row in second transaction\n";
    out_result($res,"test5");

    ibase_free_result($res);

	$res = ibase_query($tr_3, "select * from test5");
    
    echo "three rows in third transaction\n";
    out_result($res,"test5");

    ibase_free_result($res);

    ibase_close();
/*
transactions on second link
*/
    $link_1 = ibase_pconnect($test_base);
    $link_2 = ibase_pconnect($test_base);
    
	$tr_1 = ibase_trans(IBASE_DEFAULT, $link_2);  /* this default transaction also */
	$tr_2 = ibase_trans(IBASE_COMMITTED, $link_2);
    
	$res = ibase_query($tr_1, "select * from test5");
    
    echo "three rows\n";
    out_result($res,"test5");

    ibase_free_result($res);

    ibase_query($tr_1, "insert into test5 (i) values (5)");
    
	$res = ibase_query($tr_1, "select * from test5");
    
    echo "four rows\n";
    out_result($res,"test5");

    ibase_free_result($res);
    
    ibase_commit($tr_1);
    
	$res = ibase_query($tr_2, "select * from test5");
    
    echo "four rows again\n";
    out_result($res,"test5");

    ibase_free_result($res);
    
    ibase_close($link_1);
    ibase_close($link_2);

    echo "end of test\n";
?>
--EXPECT--
default transaction:
empty table
--- test5 ---
---
one row
--- test5 ---
1	
---
after rollback table empty again
--- test5 ---
---
one row
--- test5 ---
2	
---
one row
--- test5 ---
2	
---
one row... again.
--- test5 ---
2	
---
one row.
--- test5 ---
2	
---
one row
--- test5 ---
2	
---
three rows
--- test5 ---
2	
3	
4	
---
three rows again
--- test5 ---
2	
3	
4	
---
one row in second transaction
--- test5 ---
2	
---
three rows in third transaction
--- test5 ---
2	
3	
4	
---
three rows
--- test5 ---
2	
3	
4	
---
four rows
--- test5 ---
2	
3	
4	
5	
---
four rows again
--- test5 ---
2	
3	
4	
5	
---
end of test

