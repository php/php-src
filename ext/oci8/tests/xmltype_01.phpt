--TEST--
Basic XMLType test
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialization

$stmts = array(
	"drop table xtt",
	"create table xtt
		   (xt_id number, xt_spec xmltype)
		   xmltype xt_spec store as clob",
	"insert into xtt (xt_id, xt_spec) values
	  (1,
	   xmltype('<?xml version=\"1.0\"?>
		<Xt>
		  <XtId>1</XtId>
		  <Size>Big</Size>
		  <Area>12345</Area>
		  <Hardness>20</Hardness>
		  <Lip>Curved</Lip>
		  <Color>Red</Color>
		  <Nice>N</Nice>
		  <Compact>Tiny</Compact>
		  <Material>Steel</Material>
		</Xt>'))"
);

foreach ($stmts as $q) {
	$s = oci_parse($c, $q);
	$r = @oci_execute($s);
	if (!$r) {
		$m = oci_error($s);
		if ($m['code'] != 942) {  // table or view doesn't exist
			echo $m['message'], "\n";
		}
	}
}

function do_query($c)
{
	$s = oci_parse($c, 'select XMLType.getClobVal(xt_spec)
				        from xtt where xt_id = 1');
	oci_execute($s);
	$row = oci_fetch_row($s);	
	$data = $row[0]->load();
	var_dump($data);
	return($data);
}

// Check
echo "Initial Data\n";
$data = do_query($c);

// Manipulate the data using SimpleXML
$sx = simplexml_load_string($data);
$sx->Hardness = $sx->Hardness - 1;
$sx->Nice = 'Y';

// Insert changes using a temporary CLOB
$s = oci_parse($c, 'update xtt
					set xt_spec = XMLType(:clob)
					where xt_id = 1');
$lob = oci_new_descriptor($c, OCI_D_LOB);	
oci_bind_by_name($s, ':clob', $lob, -1, OCI_B_CLOB);
$lob->writeTemporary($sx->asXml()); 
oci_execute($s);	
$lob->close();

// Verify
echo "Verify\n";
$data = do_query($c);

// Cleanup

$stmts = array(
	"drop table xtt",
);

foreach ($stmts as $q) {
	$s = oci_parse($c, $q);
	@oci_execute($s);
}

echo "Done\n";

?>
--EXPECT--
Initial Data
string(250) "<?xml version="1.0"?>
		<Xt>
		  <XtId>1</XtId>
		  <Size>Big</Size>
		  <Area>12345</Area>
		  <Hardness>20</Hardness>
		  <Lip>Curved</Lip>
		  <Color>Red</Color>
		  <Nice>N</Nice>
		  <Compact>Tiny</Compact>
		  <Material>Steel</Material>
		</Xt>"
Verify
string(249) "<?xml version="1.0"?>
<Xt>
		  <XtId>1</XtId>
		  <Size>Big</Size>
		  <Area>12345</Area>
		  <Hardness>19</Hardness>
		  <Lip>Curved</Lip>
		  <Color>Red</Color>
		  <Nice>Y</Nice>
		  <Compact>Tiny</Compact>
		  <Material>Steel</Material>
		</Xt>
"
Done
