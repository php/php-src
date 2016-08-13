--TEST--
Bug #70133 (Extended SessionHandler::read is ignoring $session_id when calling parent)
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_handler=files
session.save_path=
session.use_strict_mode=0
--FILE--
<?php

class CustomReadHandler extends \SessionHandler {

    public function read($session_id) {
        return parent::read('mycustomsession');
    }
}

ob_start();

session_set_save_handler(new CustomReadHandler(), true);

session_id('mycustomsession');
session_start();
$_SESSION['foo'] = 'hoge';
var_dump(session_id());
session_commit();

session_id('otherid');
session_start();
var_dump($_SESSION);
var_dump(session_id());

?>
--EXPECT--
string(15) "mycustomsession"
array(1) {
  ["foo"]=>
  string(4) "hoge"
}
string(7) "otherid"
