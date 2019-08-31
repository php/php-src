--TEST--
PDO PgSQL Bug #75402 Possible Memory Leak using PDO::CURSOR_SCROLL option
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

$resp = new \stdClass();
$resp->entries = [];

$db->query('DROP TABLE IF EXISTS bug75402 CASCADE');
$db->query('CREATE TABLE bug75402 (
    "id" character varying(64) NOT NULL,
    "group_id" character varying(64) NOT NULL,
    "submitter" character varying(320) NOT NULL,
    "operation" character varying(32) NOT NULL,
    "description" character varying(320) NOT NULL,
    "stage" character varying(16) NOT NULL,
    "status" character varying(64) NOT NULL,
    "progress" integer NOT NULL,
    "insert_datetime" timestamp(3) NOT NULL,
    "begin_datetime" timestamp(3),
    "end_datetime" timestamp(3),
    "life_hours" integer NOT NULL,
    "family" character varying(32) NOT NULL,
    "parallelism_group" character varying(32) NOT NULL,
    "max_parallelism" integer NOT NULL,
    "hidden" boolean NOT NULL,
    "abort" boolean NOT NULL,
    "order_folder_pathname" character varying(320),
    "worker" character varying(32) NOT NULL,
    CONSTRAINT "pk_bug75402" PRIMARY KEY ("id")
) WITH (oids = false);');



$db->query("INSERT INTO bug75402 (\"id\", \"group_id\", \"submitter\", \"operation\", \"description\", \"stage\", \"status\", \"progress\", \"insert_datetime\", \"begin_datetime\", \"end_datetime\", \"life_hours\", \"family\", \"parallelism_group\", \"max_parallelism\", \"hidden\", \"abort\", \"order_folder_pathname\", \"worker\") VALUES
('20171016083645_5337',	'G_20171016083645_5337',	'TESTPetunia',	'IMPORT',	'',	'DON',	'Completed',	100,	'2017-10-16 08:36:45',	'2017-10-16 08:36:46',	'2017-10-16 08:36:46',	96,	'IMPORT',	'',	-1,	'f',	'f',	'C:\ProgramData\TestPath\TestApp\Jobs\Jobs\\20171016083645_5337',	'MainService')");



$sql = "SELECT 
            ID as \"sID\",
            GROUP_ID as \"sGroupID\",
            SUBMITTER as \"sOwner\",
            OPERATION as \"sOperation\",
            DESCRIPTION as \"sInfo\",
            STAGE as \"sShortStatus\",
            STATUS as \"sStatus\",
            PROGRESS as \"sProgress\",
            HIDDEN as \"bHidden\",
            to_char(INSERT_DATETIME, 'IYYY.MM.DD HH24:MI:SS')  as \"sDatetime\" 
          FROM bug75402 
          ORDER BY INSERT_DATETIME DESC";

if ($db) {
    $stmt = $db->prepare($sql,
		array(
			// With the following options memory is not being
			// deallocated
			  \PDO::ATTR_CURSOR => \PDO::CURSOR_SCROLL
			// With the following option memory is de-allocated
			// \PDO::ATTR_CURSOR => \PDO::CURSOR_FWDONLY
		)
    );
    $stmt->execute();

    while ($entry = $stmt->fetchObject()) {
      $resp->entries [] = $entry;
    }
    $stmt->closeCursor();
    $stmt = null;
    $db = null;
}

var_dump($resp);
--EXPECT--
object(stdClass)#2 (1) {
  ["entries"]=>
  array(1) {
    [0]=>
    object(stdClass)#4 (10) {
      ["sid"]=>
      string(19) "20171016083645_5337"
      ["sgroupid"]=>
      string(21) "G_20171016083645_5337"
      ["sowner"]=>
      string(11) "TESTPetunia"
      ["soperation"]=>
      string(6) "IMPORT"
      ["sinfo"]=>
      string(0) ""
      ["sshortstatus"]=>
      string(3) "DON"
      ["sstatus"]=>
      string(9) "Completed"
      ["sprogress"]=>
      string(3) "100"
      ["bhidden"]=>
      bool(false)
      ["sdatetime"]=>
      string(19) "2017.10.16 08:36:45"
    }
  }
}
