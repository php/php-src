--TEST--
Existence of old function aliases
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

var_dump(oci_free_cursor());
var_dump(ocifreecursor());
var_dump(ocibindbyname());
var_dump(ocidefinebyname());
var_dump(ocicolumnisnull());
var_dump(ocicolumnname());
var_dump(ocicolumnsize());
var_dump(ocicolumnscale());
var_dump(ocicolumnprecision());
var_dump(ocicolumntype());
var_dump(ocicolumntyperaw());
var_dump(ociexecute());
var_dump(ocicancel());
var_dump(ocifetch());
var_dump(ocifetchstatement());
var_dump(ocifreestatement());
var_dump(ociinternaldebug());
var_dump(ocinumcols());
var_dump(ociparse());
var_dump(ocinewcursor());
var_dump(ociresult());
var_dump(ociserverversion());
var_dump(ocistatementtype());
var_dump(ocirowcount());
var_dump(ocilogoff());
var_dump(ocilogon());
var_dump(ocinlogon());
var_dump(ociplogon());
var_dump(ocierror());
var_dump(ocifreedesc());
var_dump(ocisavelob());
var_dump(ocisavelobfile());
var_dump(ociwritelobtofile());
var_dump(ociloadlob());
var_dump(ocicommit());
var_dump(ocirollback());
var_dump(ocinewdescriptor());
var_dump(ocisetprefetch());
var_dump(ocipasswordchange());
var_dump(ocifreecollection());
var_dump(ocinewcollection());
var_dump(ocicollappend());
var_dump(ocicollgetelem());
var_dump(ocicollassignelem());
var_dump(ocicollsize());
var_dump(ocicollmax());
var_dump(ocicolltrim());

echo "Done\n";

?>
--EXPECTF--
Warning: oci_free_cursor() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocifreecursor() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocibindbyname() expects at least 3 parameters, 0 given in %s on line %d
NULL

Warning: ocidefinebyname() expects at least 3 parameters, 0 given in %s on line %d
NULL

Warning: ocicolumnisnull() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

Warning: ocicolumnname() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

Warning: ocicolumnsize() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

Warning: ocicolumnscale() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

Warning: ocicolumnprecision() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

Warning: ocicolumntype() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

Warning: ocicolumntyperaw() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

Warning: ociexecute() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: ocicancel() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocifetch() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocifetchstatement() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: ocifreestatement() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ociinternaldebug() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocinumcols() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ociparse() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: ocinewcursor() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ociresult() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

Warning: ociserverversion() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocistatementtype() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocirowcount() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocilogoff() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocilogon() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: ocinlogon() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: ociplogon() expects at least 2 parameters, 0 given in %s on line %d
NULL
bool(false)

Warning: ocifreedesc() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocisavelob() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: ocisavelobfile() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: ociwritelobtofile() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: ociloadlob() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocicommit() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocirollback() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocinewdescriptor() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: ocisetprefetch() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: Wrong parameter count for ocipasswordchange() in %s on line %d
NULL

Warning: ocifreecollection() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocinewcollection() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: ocicollappend() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: ocicollgetelem() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: ocicollassignelem() expects exactly 3 parameters, 0 given in %s on line %d
NULL

Warning: ocicollsize() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocicollmax() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ocicolltrim() expects exactly 2 parameters, 0 given in %s on line %d
NULL
Done
